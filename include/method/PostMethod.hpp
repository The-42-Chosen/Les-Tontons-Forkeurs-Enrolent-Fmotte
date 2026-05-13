/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:40 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/12 19:47:09 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AMethod.hpp"

class HttpRequest;

class PostMethod : public AMethod
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    PostMethod();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    PostMethod(HttpRequest *http_request, Location *location);
    ~PostMethod();
    PostMethod(const PostMethod &other);
    PostMethod &operator=(const PostMethod &other);

    // =====================
    // == 	  Member	  ==
    // =====================
    void applyMethod(void);
};