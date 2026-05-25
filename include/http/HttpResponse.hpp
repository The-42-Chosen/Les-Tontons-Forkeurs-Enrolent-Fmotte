/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 11:56:16 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/24 18:55:25 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class Request;

class HttpResponse
{
  	private:
		std::string _responseContent;
		Request *_request;
		
		HttpResponse();

	public:
		// =====================
		// == Canonical Form  ==
		// =====================
		HttpResponse(Request *Request);
		~HttpResponse();
		
		// =====================
		// ==     Getters     ==
		// =====================
		std::string getResponseContent();
		void setResponseContent(std::string responseContent);
		Request *getRequest(void) const;
        void setRequest(Request *Request);
		
		// =====================
		// == 	  Methods	  ==
		// =====================
		void initialisationHttpResponse(std::string payload);
		void handleError();
		void handleRedirection();
		void handleCorrect(HttpMethod method, std::string payload);
		void sendToClient();
};
