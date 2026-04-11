/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 17:58:55 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 14:56:31 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <set>
# include <sstream>
# include <algorithm>

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <fcntl.h>

# include <unistd.h>
# include <signal.h>

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
};

struct s_return
{
    int code;
    std::string value; // URL ou message
};
