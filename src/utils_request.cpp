/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:53:46 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/22 17:24:26 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils_request.hpp"


int computeRemainingCost(const std::string &string, size_t min_len, int score)
{
    for (size_t i = min_len; i < string.size(); ++i)
    {
        if (islower(string[i]))
            score += std::abs(string[i] - 'a');
        else if (isupper(string[i]))
            score += std::abs(string[i] - 'A'); 
        else
            score += string[i];
    }
    return score;
}

int longestPrefixMatch(std::string uri, std::string location)
{
    int score = 0;
    
    std::string sub_string;
    std::vector<std::string> token_str1;
    std::vector<std::string> token_str2;

    std::stringstream iss1(uri);
    while (getline(iss1, sub_string, '/'))
        token_str1.push_back(sub_string);

    std::stringstream iss2(location);
    while (getline(iss2, sub_string, '/'))
        token_str2.push_back(sub_string);

    size_t i = 0, j = 0;
    while (i < token_str1.size() || j < token_str2.size())
    {
        if (token_str1.size() == i +1 && token_str1[i].find('.') != std::string::npos)
        {
            ++i;
            continue;
        }
            
        const std::string& s1 = (i < token_str1.size()) ? token_str1[i] : "";
        const std::string& s2 = (j < token_str2.size()) ? token_str2[j] : "";
 
        size_t min_len = std::min(s1.size(), s2.size());

        for (size_t i = 0; i < min_len; ++i)
            score += std::abs(s1[i] - s2[i]);

        score += computeRemainingCost(s1, min_len, score);
        score += computeRemainingCost(s2, min_len, score);
            
        ++i;
        ++j;
    }
    return score;
}