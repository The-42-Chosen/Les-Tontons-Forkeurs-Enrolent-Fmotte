/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_duplicate.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:22 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 12:30:33 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

std::string parseRootDirective(std::vector<std::string> &tokens);
int parseAutoIndexDirective(std::vector<std::string> &tokens);
unsigned int parseClientMaxBodySizeDirective(std::vector<std::string> &tokens);
HttpReturn parseErrorPageDirective(std::vector<std::string> &tokens, bool &is_init);
HttpReturn parseReturnDirective(std::vector<std::string> &tokens, bool &is_init);