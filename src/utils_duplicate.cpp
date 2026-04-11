/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_duplicate.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:24 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 17:23:46 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "utils_duplicate.hpp"
# include "execption.hpp"

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