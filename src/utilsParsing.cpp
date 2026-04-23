/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsParsing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:21:25 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:35:39 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utilsParsing.hpp"
#include "execption.hpp"

int parseConfigFile(char *filename, std::string &content_file)
{
    // Reading
    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cout << "Error: can't open " << filename << std::endl;
        return (1);
    }

    std::string line;
    while (std::getline(f, line))
    {
        line.append("\n");
        content_file.append(line);
    }

    f.close();
    return (0);
}

std::vector<std::string> tokenizeString(std::string &content_file)
{
    std::vector<std::string> tokens;
    std::string delimiters = " \t\n\0";
    std::string tmp;
    size_t startPos = 0;
    size_t endPos = 0;

    while ((endPos = content_file.find_first_of(delimiters, startPos)) != std::string::npos)
    {
        if (endPos != startPos)
        {
            // Skip comment
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

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (countOccurrences(tokens[i], '"') == 1)
        {
            if (i + 1 < tokens.size())
            {
                tokens[i].append(" ");
                tokens[i].append(tokens[i + 1]);
                tokens.erase(tokens.begin() + i + 1);
            }
        }
        if (*(tokens[i].end() - 1) == ';' && tokens[i].size() != 1)
        {
            tokens[i] = tokens[i].substr(0, tokens[i].size() - 1);
            tokens.insert(tokens.begin() + i + 1, ";");
            i++;
        }
    }
    // for(size_t i = 0; i < tokens.size(); ++i)
    //     std::cout << tokens[i] << "\n";

    return tokens;
}

unsigned int countOccurrences(const std::string &string, const char occ)
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