/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CorrectResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 15:44:03 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/29 15:47:35 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AResponse.hpp"
#include "struct.hpp"

class CorrectResponse : public AResponse
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================

    CorrectResponse();

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    CorrectResponse(HttpResponse *httpResponse, int statusCode);
    ~CorrectResponse();

    // =====================
    // ==     Member      ==
    // =====================
    std::string getCorrectPage();
};