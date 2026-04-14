/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 14:43:11 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/14 18:32:11 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "struct.hpp"
class Client
{
  private:
    int _client_fd;
    int _server_fd;
    Server *_server;
    std::string _request;

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    Client();
    ~Client();
    Client(const Client &other);
    Client &operator=(const Client &other);

    // =====================
    // == Getter & Setter ==
    // =====================

    // CLIENT-FD
    int get_client_fd(void);
    void set_client_fd(int client_fd);

    // SERVER-FD
    int get_server_fd(void);
    void set_server_fd(int server_fd);

    // SERVER-PTR
    Server *get_server_ptr(void);
    void set_server_ptr(Server *server);

    // REQUEST
    std::string &get_request(void);
    void set_request(std::string &request);
    void append_request(std::string &request);
};
