/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 16:14:09 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/13 16:51:06 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "struct.hpp"


bool s_listen::operator<(const s_listen& other) const
{
    if (ip != other.ip)
        return ip < other.ip;
    return port < other.port;
}
