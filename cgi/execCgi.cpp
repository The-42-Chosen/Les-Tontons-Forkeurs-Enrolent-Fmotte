/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execCgi.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 00:00:00 by erpascua          #+#    #+#             */
/*   Updated: 2026/06/11 04:03:46 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

static std::string env(const char *key)
{
    const char *val = std::getenv(key);
    return val ? val : "";
}

static std::string read_stdin(size_t n)
{
    std::string body(n, '\0');
    size_t total = 0;
    while (total < n)
    {
        int r = std::cin.readsome(&body[total], static_cast<std::streamsize>(n - total));
        if (r <= 0)
            break;
        total += r;
    }
    body.resize(total);
    return body;
}

static std::string url_decode(const std::string &s)
{
    std::string out;
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '+')
            out += ' ';
        else if (s[i] == '%' && i + 2 < s.size())
        {
            char hex[3] = {s[i + 1], s[i + 2], 0};
            out += static_cast<char>(std::strtol(hex, NULL, 16));
            i += 2;
        }
        else
            out += s[i];
    }
    return out;
}

// Parse "key=value&key2=value2" → map
static std::map<std::string, std::string> parse_query(const std::string &qs)
{
    std::map<std::string, std::string> params;
    std::istringstream stream(qs);
    std::string pair;
    while (std::getline(stream, pair, '&'))
    {
        size_t eq = pair.find('=');
        if (eq == std::string::npos)
            continue;
        std::string key = url_decode(pair.substr(0, eq));
        std::string val = url_decode(pair.substr(eq + 1));
        params[key] = val;
    }
    return params;
}

// Echappe les caracteres HTML speciaux
static std::string escape_html(const std::string &s)
{
    std::string out;
    for (size_t i = 0; i < s.size(); ++i)
    {
        switch (s[i])
        {
        case '&': out += "&amp;"; break;
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '"': out += "&quot;"; break;
        default:  out += s[i]; break;
        }
    }
    return out;
}

int main()
{
    const std::string method = env("REQUEST_METHOD");

    // GET : query string depuis l'environnement
    // POST : body depuis stdin
    std::string raw_data;
    if (method == "POST")
    {
        const std::string cl = env("CONTENT_LENGTH");
        if (!cl.empty())
        {
            size_t len = static_cast<size_t>(std::atoi(cl.c_str()));
            raw_data = read_stdin(len);
        }
    }
    else
        raw_data = env("QUERY_STRING");

    std::map<std::string, std::string> params = parse_query(raw_data);

    // Headers HTTP
    std::cout << "Content-Type: text/html\r\n";
    std::cout << "\r\n";

    // Page HTML
    std::cout << "<!DOCTYPE html>\n"
              << "<html lang=\"fr\">\n"
              << "<head><meta charset=\"utf-8\"><title>CGI Webserv</title></head>\n"
              << "<body>\n"
              << "<h1>CGI Webserv</h1>\n"
              << "<p><strong>Method :</strong> " << escape_html(method) << "</p>\n";

    if (params.empty())
        std::cout << "<p>Aucun parametre recu.</p>\n";
    else
    {
        std::cout << "<h2>Parametres</h2>\n<ul>\n";
        for (std::map<std::string, std::string>::iterator it = params.begin(); it != params.end(); ++it)
            std::cout << "  <li><strong>" << escape_html(it->first) << "</strong> = "
                      << escape_html(it->second) << "</li>\n";
        std::cout << "</ul>\n";
    }

    std::cout << "<hr>\n"
              << "<p><em>Server: " << escape_html(env("SERVER_NAME")) << ":"
              << escape_html(env("SERVER_PORT")) << " | Protocol: "
              << escape_html(env("SERVER_PROTOCOL")) << "</em></p>\n"
              << "</body>\n</html>\n";

    return 0;
}
