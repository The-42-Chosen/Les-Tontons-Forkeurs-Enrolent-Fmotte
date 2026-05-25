/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:40 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/25 11:20:00 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AMethod.hpp"

class HttpRequest;

class HeadMethod : public AMethod
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================

    HeadMethod();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    HeadMethod(HttpRequest *httpRequest);
    ~HeadMethod();
    HeadMethod(const HeadMethod &other);
    HeadMethod &operator=(const HeadMethod &other);

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string applyMethod(Location *location);
};