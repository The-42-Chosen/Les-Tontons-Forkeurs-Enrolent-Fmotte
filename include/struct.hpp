/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 17:58:55 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/04 15:43:02 by fmotte           ###   ########.fr       */
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

struct s_location
{	
	std::string 						name;
	std::set<method_http> 				allowed_methods;
	std::string							root;
	std::vector<std::string> 			indexs;
	bool								auto_index;
	s_return							error_page;
	s_return							ret;
};
