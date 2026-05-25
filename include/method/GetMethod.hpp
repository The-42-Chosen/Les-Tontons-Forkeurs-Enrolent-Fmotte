/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:55:49 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/25 11:35:19 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AMethod.hpp"

class HttpRequest;

class GetMethod : public AMethod
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================

    GetMethod();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    GetMethod(HttpRequest *httpRequest);
    ~GetMethod();
    GetMethod(const GetMethod &other);
    GetMethod &operator=(const GetMethod &other);

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string applyMethod(Location *location);
};
