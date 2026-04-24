/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsParsing.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:21:21 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/24 17:02:20 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

unsigned int countOccurrences(const std::string &string, const char occ);
int parseConfigFile(const char *filename, std::string &content_file);
std::vector<std::string> tokenizeString(std::string &content_file);