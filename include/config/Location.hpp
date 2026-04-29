/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 15:45:26 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/21 16:28:52 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class Location
{
  private:
    std::string _name;
    std::set<HttpMethod> _allowed_methods;
    std::string _root;
    std::string _index;
    bool _auto_index;
    HttpReturn _error_page;
    unsigned int _client_max_body_size;
    HttpReturn _ret;

  public:
    // =====================
    // == Canonical Form  ==
    // =====================

    Location();
    ~Location();
    Location(const Location &other);
    Location &operator=(const Location &other);

    static const std::string list_allowed_methods[4];

    // =====================
    // == Getter & Setter ==
    // =====================

    // NAME
    void setName(std::string name);
    std::string getName(void);

    // METHOD-HTTP
    void addAllowedMethod(HttpMethod i);
    std::set<HttpMethod> getAllowedMethods(void);

    // INDEX
    void setIndex(std::string index);
    std::string getIndex(void);

    // ROOT
    void setRoot(std::string root);
    std::string getRoot(void);

    // AUTO-INDEX
    void setAutoIndex(bool auto_index);
    bool getAutoIndex(void);

    // CLIENT-MAX-BODY-SIZE
    void setClientMaxBodySize(unsigned int client_max_body_size);
    unsigned int getClientMaxBodySize(void);

    // ERROR-PAGE
    void setErrorPage(HttpReturn error_page);
    HttpReturn *getErrorPage(void);

    // RETURN
    void setReturn(HttpReturn ret);
    HttpReturn *getReturn(void);

    // =====================
    // ==     Method      ==
    // =====================

    void initializeLocation(std::vector<std::string> &tokens);

    void initializeLocationAllowedMethods(std::vector<std::string> &tokens);
    void initializeLocationRoot(std::vector<std::string> &tokens);

    void initializeLocationIndex(std::vector<std::string> &tokens);
    void initializeLocationAutoIndex(std::vector<std::string> &tokens);

    void initializeLocationClientMaxBodySize(std::vector<std::string> &tokens);
    void initializeLocationErrorPage(std::vector<std::string> &tokens);
    void initializeLocationReturn(std::vector<std::string> &tokens);
};
