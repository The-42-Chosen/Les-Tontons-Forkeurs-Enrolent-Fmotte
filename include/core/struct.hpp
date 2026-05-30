/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 17:58:55 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/27 13:58:16 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
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

typedef std::vector<uint8_t> BodyContent;
typedef std::map<std::string, std::string> HeaderContent;

enum HttpMethod
{
    GET,
    POST,
    DELETE,
    HEAD,
    NONE
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
    std::string value; // URL
};

struct HttpErrorPage
{
    int code;
    std::string path_page; // Absolute path to the page
};