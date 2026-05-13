/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:52:26 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/13 20:11:18 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpResponse::HttpResponse(int code, Body &body, const std::string &contentType)
    : _statusCode(code), _body(body), _statusMessage(getStatusMessage(code))
{
	if (!contentType.empty())
		_headers["content-type"] = contentType;
}

HttpResponse::~HttpResponse()
{
}

// =====================
// == 		Makers	  ==
// =====================

// OK 200
HttpResponse HttpResponse::makeFile(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::binary);

    if (!file.is_open())
        return (makeError(404));

    Body cpyBody;
    std::stringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    cpyBody.assign(content.begin(), content.end());

    file.close();

    return (HttpResponse(200, cpyBody, "text/html"));
}

// KO 400-500
HttpResponse HttpResponse::makeError(int code, const std::string &customPage)
{
    Body errorBody;
    std::string errorHtml;

    if (!customPage.empty())
    {
        std::ifstream file(customPage.c_str(), std::ios::binary);
        if (file.is_open())
        {
            std::stringstream ss;
            ss << file.rdbuf();
            errorHtml = ss.str();
            file.close();
        }
    }

    if (errorHtml.empty())
    {
        std::stringstream ss;
        ss << code;
        errorHtml = "<html><body><h1>Error " + ss.str() + "</h1><p>" + getStatusMessage(code) + "</p></body></html>";
    }

    errorBody.assign(errorHtml.begin(), errorHtml.end());
    return (HttpResponse(code, errorBody, "text/html"));
}

// OK 300
HttpResponse HttpResponse::makeRedirect(int code, const std::string &location)
{
    Body redirectBody;
    std::string html = "<html><body><a href=\"" + location + "\">Redirecting...</a></body></html>";
    redirectBody.assign(html.begin(), html.end());

    HttpResponse response(code, redirectBody, "text/html");
    response.addHeader("Location", location);
    return (response);
}

HttpResponse HttpResponse::makeAutoIndex(const std::string &dirPath, const std::string &uri)
{
    Body indexBody;
    std::string html;

    html = "<html><head><title>Index of " + uri + "</title></head><body>";
    html += "<h1>Index of " + uri + "</h1><ul>";

    DIR *dir = opendir(dirPath.c_str());
    if (!dir)
        return (makeError(403));

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        std::string entryPath = uri;
        if (uri[uri.length() - 1] != '/')
            entryPath += "/";
        entryPath += entry->d_name;

        if (entry->d_type == DT_DIR)
            entryPath += "/";

        html += "<li><a href=\"" + entryPath + "\">" + entry->d_name;
        if (entry->d_type == DT_DIR)
            html += "/";
        html += "</a></li>";
    }
    closedir(dir);

    html += "</ul></body></html>";
    indexBody.assign(html.begin(), html.end());

    return (HttpResponse(200, indexBody, "text/html"));
}

// =====================
// == 		Methods     ==
// =====================

void HttpResponse::addHeader(const std::string &key, const std::string &val)
{
	_headers[key] = val;
}

std::string HttpResponse::build() const
{
	std::stringstream ss;
    ss << _statusCode;
    std::string response = "HTTP/1.1 " + ss.str() + " " + _statusMessage + "\r\n";

	for(Header::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		response += it->first + ": " + it->second + "\r\n";
	response += "\r\n";
	response += std::string(_body.begin(), _body.end());
	
	return (response);
}

void HttpResponse::send(int fd) const
{
    std::string headers = build();
    ::send(fd, headers.c_str(), headers.size(), 0);
      if (!_body.empty())
          ::send(fd, _body.data(), _body.size(), 0);

}
