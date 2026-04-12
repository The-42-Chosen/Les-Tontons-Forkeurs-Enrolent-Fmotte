/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:58:22 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 17:22:05 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

std::string return_root(std::vector<std::string> &tokens);
int return_auto_index(std::vector<std::string> &tokens);
unsigned int return_client_max_body_size(std::vector<std::string> &tokens);
s_return return_error_page(std::vector<std::string> &tokens, bool &is_init);
s_return return_return(std::vector<std::string> &tokens, bool &is_init);