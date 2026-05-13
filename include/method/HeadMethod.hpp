/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:40 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/13 19:36:29 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AMethod.hpp"
#include <ctime>

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
    HeadMethod(HttpRequest *http_request, Location *location);
    ~HeadMethod();
    HeadMethod(const HeadMethod &other);
    HeadMethod &operator=(const HeadMethod &other);

    // =====================
    // == 	  Member	  ==
    // =====================
    void applyMethod(void);
};