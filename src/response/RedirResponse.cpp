/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:31:47 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/29 15:12:48 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RedirResponse.hpp"

RedirResponse::RedirResponse(HttpResponse *httpResponse, int statusCode) : AResponse(httpResponse, statusCode)
{
}

RedirResponse::~RedirResponse()
{
}
