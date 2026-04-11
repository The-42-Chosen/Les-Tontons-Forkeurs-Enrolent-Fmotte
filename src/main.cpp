/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 15:24:58 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 18:20:57 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "struct.hpp"
# include "webserv.hpp"
# include "utils_parsing.hpp"
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
    
    
    Webserv webser;  
    if (webser.initialisation_webserv(tokens))
		return (1);
    
    if (webser.initialisation_connection())
        return (2);
        
    return (0);
}