/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parsing.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:21:21 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 17:22:02 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

unsigned int get_nb_occurence(const std::string &string, const char occ);
int parse_config_file(char *filename, std::string &content_file);
std::vector<std::string> tokenize_string(std::string &content_file);