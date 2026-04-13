/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 17:58:55 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/13 18:34:34 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <signal.h>
#include <unistd.h>

enum method_http
{
    GET,
    POST,
    DELETE,
    HEAD
};

struct s_listen
{
    std::string ip;
    unsigned int port;

    bool operator<(const s_listen &other) const;
};

struct s_return
{
    int code;
    std::string value; // URL ou message
};

struct s_client
{
    int fd;
    std::string request;
};