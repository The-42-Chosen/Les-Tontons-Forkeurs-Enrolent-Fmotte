/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/23 19:15:59 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"
#include <ctime>
#include <exception>
#include <map>
#include <set>

#define MAX_CLIENT 10
#define MAX_EVENTS 10
#define SIZE_BUFFER 1024
#define SESSION_TTL 3600
#define DELAY 5000

class Server;
class Client;

struct SessionInfo
{
    int visits;
    time_t lastSeen;
};

enum RequestState
{
    REQUEST_DISCONNECTED,
    REQUEST_INCOMPLETE,
    REQUEST_COMPLETE
};

class Webserv
{
  private:
    std::vector<Server *> _vectorServer;
    std::vector<Client *> _vectorClient;
    std::map<int, std::set<Server *> > _mapFdToServer;
    std::map<std::string, SessionInfo> _sessions;
    std::set<EventData*> _setEventData;
    
    int _webserEpoll;

    void cleanupSessions(void);

  public:
    // =====================
    // == Canonical Form  ==
    // =====================

    Webserv();
    ~Webserv();
    Webserv(const Webserv &other);
    Webserv &operator=(const Webserv &other);

    // =====================
    // == Getter & Setter ==
    // =====================

    // SERVERS
    const std::vector<Server *> &getServers(void) const;
    const std::vector<Client *> &getClients(void) const;
    const std::map<int, std::set<Server *> > &getFdToServersMap(void) const;
    void setEpollFd(const int epoll);
    int getEpollFd(void);
    std::set<EventData*> getSetEventData(void) const;
    void addSetEventData(EventData* eventData);
    
    // SESSION
    int touchSession(const std::string &sessionId);

    // =====================
    // ==     Method      ==
    // =====================

    bool initializeWebserv(std::vector<std::string> &tokens);
    bool splitIntoServers(std::vector<std::string> &tokens);

    bool initializeConnection();
    void initializeSocket();
    void registerNewSocket(std::map<Listen, int> &map_socket_fd, Listen *listenConfig, Server *server);
    void registerExistingSocket(int serverSocket, Server *server);
    void listenToWebserv();

    void handleConnection(struct epoll_event &events);
    void handleNewClient(int server_fd);

    void processClient(EventData *eventData);
    void processClientResponse(Client *client);
    void applyErrorToResponse(Client *client, const std::exception &e);
    void sendResponseToClient(Client *client);

    void writeToChild(EventData *eventData);
    void readToChild(EventData *eventData);

    RequestState readAndCheckRequestCompletion(Client *client);
    void handleDisconnect(Client *client);
    void deleteClient(Client *client);
    void closeConnection();

    void checkTimeOut();
};
