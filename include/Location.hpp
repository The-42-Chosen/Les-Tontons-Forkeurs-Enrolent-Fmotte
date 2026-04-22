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
    std::set<method_http> _allowed_methods;
    std::string _root;
    std::string _index;
    bool _auto_index;
    s_return _error_page;
    unsigned int _client_max_body_size;
    s_return _ret;

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
    void set_name(std::string name);
    std::string get_name(void);

    // METHOD-HTTP
    void add_methode_http(method_http i);
    std::set<method_http> get_methode_http(void);

    // INDEX
    void set_index(std::string index);
    std::string get_index(void);

    // ROOT
    void setRoot(std::string root);
    std::string getRoot(void);

    // AUTO-INDEX
    void set_auto_index(bool auto_index);
    bool get_auto_index(void);

    // CLIENT-MAX-BODY-SIZE
    void set_client_max_body_size(unsigned int client_max_body_size);
    unsigned int get_client_max_body_size(void);

    // ERROR-PAGE
    void set_error_page(s_return error_page);
    s_return *get_error_page(void);

    // RETURN
    void set_return(s_return ret);
    s_return *get_return(void);

    // =====================
    // ==     Method      ==
    // =====================

    void init_location(std::vector<std::string> &tokens);

    void init_location_allowed_methods(std::vector<std::string> &tokens);
    void init_location_root(std::vector<std::string> &tokens);

    void init_location_index(std::vector<std::string> &tokens);
    void init_location_auto_index(std::vector<std::string> &tokens);

    void init_location_client_max_body_size(std::vector<std::string> &tokens);
    void init_locatoin_error_page(std::vector<std::string> &tokens);
    void init_location_return(std::vector<std::string> &tokens);
};
