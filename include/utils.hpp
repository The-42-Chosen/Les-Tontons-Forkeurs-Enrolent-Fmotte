/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:22 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/09 13:26:48 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "struct.hpp"

unsigned int get_nb_occurence(const std::string &string, const char occ);
std::string return_root(std::vector <std::string> &tokens);
int return_auto_index(std::vector <std::string> &tokens);
unsigned int return_client_max_body_size(std::vector <std::string> &tokens);
s_return return_error_page(std::vector <std::string> &tokens, bool &is_init);
s_return return_return(std::vector <std::string> &tokens, bool &is_init);
int parse_config_file(char *filename, std::string &content_file);
std::vector <std::string> tokenize_string(std::string &content_file);