/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:35:48 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/29 15:07:22 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "execption.hpp"

class Server;
class HttpRequest;
class Client;
class Location;

class Request
{
    private:
        // =====================
		// ==    Attributs    ==
		// =====================
		Client *_client;
		Server *_server;
        HttpRequest *_httpRequest;
		int _statusCode;
		Location *_location;
		std::string _payload;
		
    public:
        // =====================
		// ==       OCF       ==
		// =====================
		Request();
		Request(const Request &cpy);
		Request &operator=(const Request &cpy);
        ~Request();
        
		// =====================
		// ==     Getters     ==
		// =====================
		Client *getClient(void) const;
		void setClient(Client *client);
		Server *getServer(void) const;
		void setServer(Server *server);
        HttpRequest *getHttpRequest(void) const;
        void setHttpRequest(HttpRequest *httpRequest);
		int getStatusCode() const;
		void setStatusCode(int statusCode);
		Location *getLocation(void) const;
    	void setLocation(Location *location);
		std::string getPayload() const;
		void setPayload(std::string payload);
		
		// =====================
		// == 	  Member	  ==
		// =====================
		bool initialisationRequest(Client *client);
		void processRequest();
		void linkToServer(void);
		void validateRequest(Location *location);
		Location *findLocation(void);
		void checkAllowedMethods(Location *location);
		void checkServerIsOpen();
		void checkLocationIsOpen(Location *location);
};
  
// =====================
// == 	  Fonction    ==
// =====================
bool isCompleteRequest(const std::string &request);
bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart);
bool isFinalChunkComplete(const std::string &request, std::string::size_type current);
std::string initSizeToken(const std::string &request, const std::string::size_type &current, const std::string::size_type &lineEnd);
std::string getHeaderValue(const std::string &request, const std::string &headerName);
bool parseDecimalLength(const std::string &value, size_t &contentLength);


