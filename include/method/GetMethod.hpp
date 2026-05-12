/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:55:49 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/12 16:44:21 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "AMethod.hpp"

class HttpRequest;

class GetMethod: public AMethod
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
        GetMethod(HttpRequest *http_request, Location *location);
        ~GetMethod();
        GetMethod(const GetMethod &other);
        GetMethod& operator=(const GetMethod& other);

        // =====================
        // == 	  Member	  ==
        // =====================
        void applyMethod(void);
};

