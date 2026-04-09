/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:24 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/09 16:34:29 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "utils.hpp"
# include "execption.hpp"

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
        line.append("\n");
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

std::string return_root(std::vector <std::string> &tokens)
{
    if (tokens[0] == "root")
    {
        tokens.erase(tokens.begin());
        std::string root = tokens[0];
        tokens.erase(tokens.begin());

        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();
            
        tokens.erase(tokens.begin());
        return root;
    }
    return "";
}

int return_auto_index(std::vector <std::string> &tokens)
{
    if (tokens[0] == "autoindex")
    {
        tokens.erase(tokens.begin());
        if (tokens[0] == "on" || tokens[0] == "true")
        {
            tokens.erase(tokens.begin());
            if (tokens[0] != ";")
                throw ExecptionMissSemiColon();
                
            tokens.erase(tokens.begin());
            return 1;
        }
            
        else if (tokens[0] == "off" || tokens[0] == "false")
        {
            tokens.erase(tokens.begin());
            if (tokens[0] != ";")
                throw ExecptionMissSemiColon();
                
            tokens.erase(tokens.begin());
            return 0;
        }
        else
            throw ExecptionWrongArgument(tokens[0]);
    }
    return -1;
}

unsigned int return_client_max_body_size(std::vector <std::string> &tokens)
{
    unsigned int client_max_body_size;
    
    if (tokens[0] == "client_max_body_size")
    {
        tokens.erase(tokens.begin());
        
        std::istringstream convert(tokens[0]);
        convert >> client_max_body_size;
        
        if (convert.fail())
            throw ExecptionFailConvertion(tokens[0]);
        
        tokens.erase(tokens.begin());
        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();
            
        tokens.erase(tokens.begin());
        return client_max_body_size;
    }
    return 0;
}

s_return return_error_page(std::vector <std::string> &tokens, bool &is_init)
{
    s_return error_page;
    
    if (tokens[0] == "error_page")
    {
        tokens.erase(tokens.begin());
        
        std::istringstream convert(tokens[0]);
        convert >> error_page.code;
         
        if (convert.fail())
            throw ExecptionFailConvertion(tokens[0]);
        
        tokens.erase(tokens.begin());
        error_page.value = tokens[0];
        tokens.erase(tokens.begin());

        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();
            
        tokens.erase(tokens.begin());
        is_init = true;
        return error_page;
    }
    return error_page;
}

s_return return_return(std::vector <std::string> &tokens, bool &is_init)
{   
    s_return ret;

    if (tokens[0] == "return")
    {
        tokens.erase(tokens.begin());
        
        std::istringstream convert(tokens[0]);
        convert >> ret.code;
         
        if (convert.fail())
           throw ExecptionFailConvertion(tokens[0]);
        
        tokens.erase(tokens.begin());
        ret.value = tokens[0];
        tokens.erase(tokens.begin());
        
        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();
            
        tokens.erase(tokens.begin());
        is_init = true;
        return ret;
    }
    return ret;
}