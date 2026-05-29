/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsDuplicate.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:22 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/27 13:30:21 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

std::string parseRootDirective(std::vector<std::string> &tokens);
int parseAutoIndexDirective(std::vector<std::string> &tokens);
unsigned int parseClientMaxBodySizeDirective(std::vector<std::string> &tokens);
HttpErrorPage parseErrorPageDirective(std::vector<std::string> &tokens, bool &is_init);
HttpReturn parseReturnDirective(std::vector<std::string> &tokens, bool &is_init);
std::string intToString(int value);
std::string trimSpaces(const std::string &value);
std::string toLowerString(const std::string &str);