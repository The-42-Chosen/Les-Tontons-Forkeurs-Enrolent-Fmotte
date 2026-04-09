/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 15:24:58 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/09 14:26:50 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "struct.hpp"
# include "server.hpp"
# include "utils.hpp"



int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "Error: Wrong number of argument\n";
        return (1);
    }

    std::string content_file;
    if (parse_config_file(argv[1], content_file))
        return (1);
    
    std::vector <std::string> tokens;
    tokens = tokenize_string(content_file);
    
    Server server;  
    if (!server.initialisation_webserv(tokens))
		return (1);
        
    return (0);
}