/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 02:50:08 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"
#include <ctime>
#include <map>
#include <set>

#define MAX_CLIENT 10
#define MAX_EVENTS 10
#define SIZE_BUFFER 1024
#define SESSION_TTL 3600

class Server;
class Client;

struct SessionInfo
{
    int visits;
    time_t lastSeen;
};

class Webserv
{
	private:
		std::vector<Server *> _vectorServer;
		std::vector<Client *> _vectorClient;
		std::map<int, std::set<Server *> > _mapFdToServer;
		std::map<std::string, SessionInfo> _sessions;
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
		bool processClientRequest(Client *client);
		void processClientResponse(Client *client);
		bool readAndCheckRequestCompletion(Client *client);
		void deleteClient(Client *client);
		void closeConnection();
};
