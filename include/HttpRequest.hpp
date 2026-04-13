/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/13 19:48:16 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <map>
# include <vector>
# include "struct.hpp"

class HttpRequest
{
	private:
	// Attributs
		HttpRequest();
		method_http _method;
		std::string _uri;
		std::string _protocol;
		std::map<std::string, std::string> _headers;
		std::vector<__uint8_t> _body;
		bool _keepAlive;
		size_t _contentLength;

	public:
	// OCF
		HttpRequest(std::string requestRawContent);
		HttpRequest(const HttpRequest& cpy);
		HttpRequest& operator=(const HttpRequest& cpy);
		~HttpRequest();

	// Member Functions
		HttpRequest& parseHeaderMethod(const std::string& headerContent);
		HttpRequest& parseHeader(const std::string& headerContent);

	// Getter
		method_http getMethod() const;
		const std::string& getUri() const;
		const std::string& getProtocol() const;

	// Helper
		static const char* methodToString(method_http method);
};
