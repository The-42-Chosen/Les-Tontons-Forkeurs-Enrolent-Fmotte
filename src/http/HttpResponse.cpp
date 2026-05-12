/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:52:26 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/12 20:14:01 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpResponse::HttpResponse(int code, Body &body, const std::string &contentType) : _statusCode(code), _body(body)
{
	if (!contentType.empty())
		_headers["content-type"] = contentType;
}

HttpResponse::~HttpResponse()
{
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
	std::string headers = build(); // build() sans le body
    ::send(fd, headers.c_str(), headers.size(), 0);
      if (!_body.empty())
          ::send(fd, _body.data(), _body.size(), 0);

}

// =====================
// == 		Makers	  ==
// =====================

HttpResponse HttpResponse::makeFile(const std::string &path)
{
	
}

HttpResponse HttpResponse::makeError(int code, const std::string &customPage = "")
{
}

HttpResponse HttpResponse::makeRedirect(int code, const std::string &location)
{
}

HttpResponse HttpResponse::makeAutoIndex(const std::string &dirPath, const std::string &uri)
{
}
