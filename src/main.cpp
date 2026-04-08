/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 15:24:58 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/08 13:46:56 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "struct.hpp"
# include "server.hpp"
# include "utils.hpp"

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
        if (*(tokens[i].end() - 1) == ';' && tokens[i].size() != 1)
        {
            tokens[i] = tokens[i].substr(0, tokens[i].size()-1);
            tokens.insert(tokens.begin() + i + 1, ";");
            i++;
        }
    }
    
    // for(size_t i = 0; i < tokens.size(); ++i)
    //     std::cout << tokens[i] << "\n";
        
    return tokens;
}

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
    server.initialisation_webserv(tokens);
        
    return (0);
}