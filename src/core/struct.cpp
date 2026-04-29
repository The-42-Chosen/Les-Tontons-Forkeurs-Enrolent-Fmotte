/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 16:14:09 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:35:39 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "struct.hpp"

bool Listen::operator<(const Listen &other) const
{
    if (ip != other.ip)
        return ip < other.ip;
    return port < other.port;
}
