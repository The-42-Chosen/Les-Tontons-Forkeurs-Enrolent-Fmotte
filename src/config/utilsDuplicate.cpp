/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsDuplicate.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:24 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:35:39 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utilsDuplicate.hpp"
#include "execption.hpp"

std::string parseRootDirective(std::vector<std::string> &tokens)
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

int parseAutoIndexDirective(std::vector<std::string> &tokens)
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

unsigned int parseClientMaxBodySizeDirective(std::vector<std::string> &tokens)
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

HttpReturn parseErrorPageDirective(std::vector<std::string> &tokens, bool &is_init)
{
    HttpReturn error_page;

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

HttpReturn parseReturnDirective(std::vector<std::string> &tokens, bool &is_init)
{
    HttpReturn ret;

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