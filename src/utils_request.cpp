/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_request.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:53:46 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/22 11:15:47 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils_request.hpp"

int longestPrefixMatch(std::string string1, std::string string2)
{
    size_t i;
    int score = 0;

    std::string sub_string;
    std::vector<std::string> token_str1;
    std::vector<std::string> token_str2;

    std::stringstream iss1(string1);
    while (getline(iss1, sub_string, '/'))
        token_str1.push_back(sub_string);

    std::stringstream iss2(string2);
    while (getline(iss2, sub_string, '/'))
        token_str2.push_back(sub_string);

    while (!token_str1.empty() || !token_str2.empty())
    {
        const std::string s1 = token_str1.empty() ? "" : token_str1.front();
        const std::string s2 = token_str2.empty() ? "" : token_str2.front();

        size_t min_len = std::min(s1.size(), s2.size());

        // Comparaison caractère par caractère
        for (size_t i = 0; i < min_len; ++i)
            score += std::abs(s1[i] - s2[i]);

        // Ajouter les caractères restants
        for (size_t i = min_len; i < s1.size(); ++i)
            score += s1[i];

        for (size_t i = min_len; i < s2.size(); ++i)
            score += s2[i];

        if (!token_str1.empty())
            token_str1.erase(token_str1.begin());
        if (!token_str2.empty())
            token_str2.erase(token_str2.begin());
    }

    std::cout << "Score: " << score << "\n";
    return score;
}