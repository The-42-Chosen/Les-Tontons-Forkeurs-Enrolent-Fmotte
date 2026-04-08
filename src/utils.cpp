/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:24 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/08 15:22:35 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "utils.hpp"
# include "execption.hpp"

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