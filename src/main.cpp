/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 15:24:58 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/10 14:47:14 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "struct.hpp"
# include "server.hpp"
# include "utils.hpp"
# include "colors.hpp"

int stop_webserv = 0;

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
    
    // TEST PRINT SERVERS
    std::cout << RED << "\n=== RÉSUMÉ DES SERVEURS PARSÉS ===\n"  << RESET;
    for (size_t i = 0; i < server.get_servers_count(); i++)
    {
        Server* srv = server.get_server(i);
        if (srv != NULL)
            std::cout << "Serveur " << (i + 1) << " ajouté au vector\n";
    }
    std::cout << "Total: " << server.get_servers_count() << " serveur(s)\n";
    std::cout << "==================================\n\n";
        
    return (0);
}