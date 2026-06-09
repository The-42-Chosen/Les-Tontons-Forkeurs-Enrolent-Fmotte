/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execCgi.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 10:33:47 by erpascua          #+#    #+#             */
/*   Updated: 2026/06/09 19:29:37 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <array>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace
{

std::optional<std::string> env(std::string_view key)
{
    if (const char *value = std::getenv(std::string(key).c_str()))
        return std::string(value);
    return std::nullopt;
}

// SECURITY!
std::string escape_html(std::string_view input)
{
    std::string out;
    out.reserve(input.size());
    for (const char c : input)
    {
        switch (c)
        {
        case '&':
            out += "&amp;";
            break;
        case '<':
            out += "&lt;";
            break;
        case '>':
            out += "&gt;";
            break;
        case '"':
            out += "&quot;";
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

}

int main()
{
    const auto method = env("REQUEST_METHOD").value_or("GET");
    const auto query = env("QUERY_STRING").value_or("");

    constexpr std::array<std::string_view, 3> cgi_vars = {
        "SERVER_PROTOCOL",
        "CONTENT_TYPE",
        "CONTENT_LENGTH",
    };

    std::cout << "<!DOCTYPE html>\n"
              << "<html lang=\"fr\">\n"
              << "<head><meta charset=\"utf-8\"><title>Welcome to the fabilous CGI</title></head>\n"
              << "<body>\n"
              << "  <h1>Welcome to the fabilous CGI</h1>\n"
              << "  <p>A CGI created by Fmotte & Erpascua</p>\n"
              << "  <p>Query string : " << escape_html(query) << "</p>\n"
              << "  <ul>\n";

    for (const auto var : cgi_vars)
    {
        const auto value = env(var).value_or("(non defini)");
        std::cout << "    <li>" << escape_html(var) << " = " << escape_html(value) << "</li>\n";
    }

    std::cout << "  </ul>\n"
              << "</body>\n"
              << "</html>\n";

    return 0;
}
