/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 00:33:36 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 04:27:44 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ARequest.hpp"

class HttpRequest;

class StaticRequest: public ARequest
{
    private:
        // =====================
        // ==    Attributs    ==
        // =====================
        
        StaticRequest();
        
    public:
        // =====================
        // ==       OCF       ==
        // =====================
        StaticRequest(ARequest arequest);
        ~StaticRequest();

        // =====================
        // ==     Method      ==
        // =====================
        void selectMethodHttp();
};

