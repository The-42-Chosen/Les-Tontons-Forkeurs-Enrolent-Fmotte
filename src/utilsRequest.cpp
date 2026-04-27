/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:53:46 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/27 16:48:33 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utilsRequest.hpp"

int computeRemainingCost(const std::string &string, size_t min_len, int score)
{
    for (size_t i = min_len; i < string.size(); ++i)
        score += 1;

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
        if (token_str1.size() == i + 1 && token_str1[i].find('.') != std::string::npos) //avoid to compare with the file if it exist
        {
            ++i;
            continue;
        }

        const std::string &s1 = (i < token_str1.size()) ? token_str1[i] : "";
        const std::string &s2 = (j < token_str2.size()) ? token_str2[j] : "";

        size_t min_len = std::min(s1.size(), s2.size());

        for (size_t i = 0; i < min_len; ++i)
        {
            if (s1[i] != s2[i])
                score += 1;
        }

        score += computeRemainingCost(s1, min_len, score);
        score += computeRemainingCost(s2, min_len, score);

        ++i;
        ++j;
    }
    return score;
}

std::string toLowerString(const std::string &str)
{
    std::string result = str;
    for (std::string::size_type i = 0; i < result.size(); ++i)
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    return result;
}

static std::string trimChunkSizeToken(const std::string &token)
{
    std::string::size_type begin = token.find_first_not_of(" \t");
    if (begin == std::string::npos)
        return ("");
    std::string::size_type end = token.find_last_not_of(" \t");
    return (token.substr(begin, end - begin + 1));
}

size_t parseChunkSize(const std::string &line)
{
    std::string sizeToken = line;
    std::string::size_type semicolon = sizeToken.find(';');
    if (semicolon != std::string::npos)
        sizeToken = sizeToken.substr(0, semicolon);
    sizeToken = trimChunkSizeToken(sizeToken);
    if (sizeToken.empty())
        throw std::runtime_error("400 Bad Request");

    std::stringstream ss(sizeToken);
    size_t chunkSize = 0;
    ss >> std::hex >> chunkSize;
    if (ss.fail())
        throw std::runtime_error("400 Bad Request");
    if (!ss.eof())
        throw std::runtime_error("400 Bad Request");
    return (chunkSize);
}

std::string returnLastElementPath(std::string path)
{
    std::string sub_string;
    std::vector<std::string> token_str;

    std::stringstream iss1(path);
    while (getline(iss1, sub_string, '/'))
        ;
    return sub_string;
}