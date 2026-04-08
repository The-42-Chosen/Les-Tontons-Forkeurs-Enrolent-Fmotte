/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execption.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 13:08:22 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/08 16:28:53 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "execption.hpp"

const char* ExecptionMissSemiColon::what() const throw() {return "Error: Miss semi-colon in the config file";}

const char* ExecptionMissBrace::what() const throw() {return "Error: Miss curly brace in the config file";}

ExecptionWrongArgument::ExecptionWrongArgument(const std::string& argument): _message("Error: Cannont interprete this argument: " + argument) {}
ExecptionWrongArgument::~ExecptionWrongArgument() throw() {}
const char* ExecptionWrongArgument::what() const throw() {return _message.c_str();}


ExecptionIllegalMethod::ExecptionIllegalMethod(const std::string& argument): _message("Error: Illegal method http: " + argument) {}
ExecptionIllegalMethod::~ExecptionIllegalMethod() throw() {}
const char* ExecptionIllegalMethod::what() const throw() {return _message.c_str();}

ExecptionFailConvertion::ExecptionFailConvertion(const std::string& argument): _message("Error: Can't convert (" + argument + ") to unsigned int") {}
ExecptionFailConvertion::~ExecptionFailConvertion() throw() {}
const char* ExecptionFailConvertion::what() const throw() {return _message.c_str();}

ExecptionMissElement::ExecptionMissElement(const std::string& argument): _message("Error: Miss this mandatory part in the config file: " + argument) {}
ExecptionMissElement::~ExecptionMissElement() throw() {}
const char* ExecptionMissElement::what() const throw() {return _message.c_str();}