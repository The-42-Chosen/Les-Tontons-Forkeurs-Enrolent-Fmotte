/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteMethod.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:55:49 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/25 11:34:46 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AMethod.hpp"

class HttpRequest;

class DeleteMethod : public AMethod
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================

    DeleteMethod();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    DeleteMethod(HttpRequest *httpRequest);
    ~DeleteMethod();
    DeleteMethod(const DeleteMethod &other);
    DeleteMethod &operator=(const DeleteMethod &other);

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string applyMethod(Location *location);
};
