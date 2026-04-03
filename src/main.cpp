/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 15:24:58 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/03 18:43:50 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"

int parse_config_file(char *filename, std::string &content_file)
{
    //Reading
    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cout << "Error: can't open " << filename << std::endl;
        return (1);
    }
    
    std::string line;
    while (std::getline(f, line))
    {
        if (!f.eof())
            line.append("\n");
        else
            break;
        content_file.append(line);
    }
    
    f.close();
    return (0);
}

unsigned int get_nb_occurence(const std::string &string, const char occ)
{
    unsigned int nb_occ = 0;
    unsigned int i = 0;

    while (string[i] != '\0')
    {
        if (string[i] == occ)
            ++nb_occ;
        ++i;
    }
    return nb_occ;
}

std::vector <std::string> tokenize_string(std::string &content_file)
{   
    std::vector <std::string> tokens;
    std::string delimiters = " \t\n\0";
    std::string tmp;
    size_t startPos = 0;
    size_t endPos = 0;

    while ((endPos = content_file.find_first_of(delimiters, startPos)) != std::string::npos)
    {
        if (endPos != startPos)
        {
            //Skip comment
            if (content_file[startPos] == '#')
            {
                while (content_file[endPos] != '\n')
                    ++endPos;
            }
            else
                tokens.push_back(content_file.substr(startPos, endPos - startPos));
        }
         
        startPos = endPos + 1; 
    }

    if (startPos != content_file.length())
        tokens.push_back(content_file.substr(startPos));

    
    for(size_t i = 0; i < tokens.size(); ++i)
    {
        if (get_nb_occurence(tokens[i], '"') == 1)
        {
            if (i + 1 < tokens.size())
            {
                tokens[i].append(" ");
                tokens[i].append(tokens[i+ 1]);
                tokens.erase(tokens.begin() + i + 1);
            }  
        }
    }
    return tokens;
}

void init_serveur(std::vector <std::string> tokens)
{
    s_server server;
    size_t i = 0;
    
    while (i < tokens.size())
    {
        std::cout << tokens[i] << '\n';

        if (tokens[i] == "server_name")
        {
            while (*(tokens[i].end() -1)!= ';')
            {
                ++i;
                server.name_servers.push_back(tokens[i]);   
            }
        }
        ++i;
    }
    for(size_t i = 0; i < server.name_servers.size(); ++i)
        std::cout << "Server Name: " << server.name_servers[i] << "\n";
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "Error: Wrong number of argument\n";
        return (1);
    }

    std::string content_file;
    parse_config_file(argv[1], content_file);
    
    std::vector <std::string> tokens;
    tokens = tokenize_string(content_file);
    
    init_serveur(tokens);
        
    return (0);
}