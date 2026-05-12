/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteMethod.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:55:49 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/12 16:44:33 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "AMethod.hpp"

class HttpRequest;

class DeleteMethod: public AMethod
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
        DeleteMethod(HttpRequest *http_request, Location *location);
        ~DeleteMethod();
        DeleteMethod(const DeleteMethod &other);
        DeleteMethod& operator=(const DeleteMethod& other);

        // =====================
        // == 	  Member	  ==
        // =====================
        void applyMethod(void);
};

