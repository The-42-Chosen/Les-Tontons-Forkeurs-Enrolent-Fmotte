/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsRequest.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:53:22 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/27 16:48:12 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

int computeRemainingCost(const std::string &string, size_t min_len, int score);
int longestPrefixMatch(std::string string1, std::string string2);
std::string toLowerString(const std::string &str);
size_t parseChunkSize(const std::string &line);
std::string returnLastElementPath(std::string path);
