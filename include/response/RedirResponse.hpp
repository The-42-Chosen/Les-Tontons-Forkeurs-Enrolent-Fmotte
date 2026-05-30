/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 14:31:45 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/29 15:42:45 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AResponse.hpp"

class RedirResponse : public AResponse
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================

    RedirResponse();

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    RedirResponse(HttpResponse *httpResponse, int statusCode);
    ~RedirResponse();
};
