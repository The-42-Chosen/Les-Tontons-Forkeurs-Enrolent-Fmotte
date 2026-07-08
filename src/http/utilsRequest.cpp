/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:53:46 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/08 21:56:43 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utilsRequest.hpp"
#include "utilsDuplicate.hpp"

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
        if (token_str1.size() == i + 1 &&
            token_str1[i].find('.') != std::string::npos) // avoid to compare with the file if it exist
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
        throw std::runtime_error("400");

    std::stringstream ss(sizeToken);
    size_t chunkSize = 0;
    ss >> std::hex >> chunkSize;
    if (ss.fail())
        throw std::runtime_error("400");
    if (!ss.eof())
        throw std::runtime_error("400");
    return (chunkSize);
}

std::string returnLastElementPath(std::string path)
{
    std::string sub_string;
    std::vector<std::string> token_str;

    // Avoid index redirection when Query is present, parse '?'!!
    std::string::size_type qpos = path.find('?');
    if (qpos != std::string::npos)
        path = path.substr(0, qpos);

    std::stringstream iss1(path);
    while (getline(iss1, sub_string, '/'))
        ;
    return sub_string;
}

void checkPermisionReadFile(std::string path)
{
    if (access(path.c_str(), F_OK) == -1)
        throw std::runtime_error("404");

    if (access(path.c_str(), R_OK) == -1)
        throw std::runtime_error("403");
}

bool isFinishByFile(std::string path)
{
    struct stat buff;

    if (access(path.c_str(), F_OK) == -1)
        return false;

    if (stat(path.c_str(), &buff) != 0)
        return false;

    if (S_ISREG(buff.st_mode))
        return true;

    return false;
}

// Code d'Eric
// C'est pas le mien
bool isCompleteRequest(const std::string &request)
{
    std::string::size_type headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return (false);

    std::string::size_type bodyStart = headerEnd + 4;
    std::string transferEncoding = getHeaderValue(request, "transfer-encoding");
    transferEncoding = toLowerString(trimSpaces(transferEncoding));
    if (transferEncoding == "chunked")
        return (isCompleteChunkedBody(request, bodyStart));

    std::string contentLengthValue = getHeaderValue(request, "content-length");
    if (!contentLengthValue.empty())
    {
        size_t contentLength = 0;
        if (!parseDecimalLength(contentLengthValue, contentLength))
            return (true);
        return ((request.size() - bodyStart) >= contentLength);
    }
    return (true);
}

bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart)
{
    std::string::size_type current = bodyStart;

    while (current < request.size())
    {
        std::string::size_type lineEnd = request.find("\r\n", current);
        if (lineEnd == std::string::npos)
            return (false);

        std::string sizeToken = initSizeToken(request, current, lineEnd);

        std::stringstream ss(sizeToken);
        size_t chunkSize = 0;
        ss >> std::hex >> chunkSize;
        if (sizeToken.empty() || ss.fail() || !ss.eof())
            return (true);

        current = lineEnd + 2;
        if (chunkSize == 0)
            return (isFinalChunkComplete(request, current));

        if (current + chunkSize + 2 > request.size())
            return (false);

        if (request.substr(current + chunkSize, 2) != "\r\n")
            return (true);

        current += chunkSize + 2;
    }
    return (false);
}

bool isFinalChunkComplete(const std::string &request, std::string::size_type current)
{
    if (request.size() < current + 2)
        return (false);

    if (request.substr(current, 2) == "\r\n")
        return (true);

    std::string::size_type trailersEnd = request.find("\r\n\r\n", current);
    return (trailersEnd != std::string::npos);
}

std::string initSizeToken(const std::string &request, const std::string::size_type &current,
                          const std::string::size_type &lineEnd)
{
    std::string sizeToken = request.substr(current, lineEnd - current);
    std::string::size_type semicolon = sizeToken.find(';');

    if (semicolon != std::string::npos)
        sizeToken = sizeToken.substr(0, semicolon);

    sizeToken = trimSpaces(sizeToken);
    return sizeToken;
}

bool parseDecimalLength(const std::string &value, size_t &contentLength)
{
    std::stringstream ss(value);
    size_t parsed = 0;

    if (!(ss >> parsed))
        return (false);

    if (!ss.eof())
        return (false);

    contentLength = parsed;
    return (true);
}

std::string getHeaderValue(const std::string &request, const std::string &headerName)
{
    std::string::size_type headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return ("");

    std::string loweredHeaderName = toLowerString(headerName);
    std::string::size_type current = request.find("\r\n");
    if (current == std::string::npos)
        return ("");
    current += 2;

    while (current < headerEnd)
    {
        std::string::size_type lineEnd = request.find("\r\n", current);
        if (lineEnd == std::string::npos || lineEnd > headerEnd)
            break;

        std::string line = request.substr(current, lineEnd - current);
        std::string::size_type colon = line.find(':');
        if (colon != std::string::npos)
        {
            std::string key = toLowerString(trimSpaces(line.substr(0, colon)));
            if (key == loweredHeaderName)
                return (trimSpaces(line.substr(colon + 1)));
        }
        current = lineEnd + 2;
    }
    return ("");
}