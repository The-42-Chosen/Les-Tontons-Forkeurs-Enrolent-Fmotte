/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 17:58:55 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 12:26:05 by erpascua         ###   ########.fr       */
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

enum HttpMethod
{
    GET,
    POST,
    DELETE,
    HEAD
};

struct Listen
{
    std::string ip;
    unsigned int port;

    bool operator<(const Listen &other) const;
};

struct HttpReturn
{
    int code;
    std::string value; // URL ou message
};
