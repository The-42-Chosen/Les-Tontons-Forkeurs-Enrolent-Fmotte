/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execption.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 13:08:22 by fmotte            #+#    #+#             */
/*   Updated: 2026/08/05 19:10:59 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execption.hpp"

const char *ExecptionMissSemiColon::what() const throw()
{
    return "Error: Miss semi-colon in the config file";
}

const char *ExecptionMissBrace::what() const throw()
{
    return "Error: Miss curly brace in the config file";
}

ExecptionWrongArgument::ExecptionWrongArgument(const std::string &argument)
    : _message("Error: Cannont interprete this argument: " + argument)
{
}
ExecptionWrongArgument::~ExecptionWrongArgument() throw()
{
}
const char *ExecptionWrongArgument::what() const throw()
{
    return _message.c_str();
}

ExecptionIllegalMethod::ExecptionIllegalMethod(const std::string &argument)
    : _message("Error: Illegal method http: " + argument)
{
}
ExecptionIllegalMethod::~ExecptionIllegalMethod() throw()
{
}
const char *ExecptionIllegalMethod::what() const throw()
{
    return _message.c_str();
}

ExecptionFailConvertion::ExecptionFailConvertion(const std::string &argument)
    : _message("Error: Can't convert (" + argument + ") to unsigned int")
{
}
ExecptionFailConvertion::~ExecptionFailConvertion() throw()
{
}
const char *ExecptionFailConvertion::what() const throw()
{
    return _message.c_str();
}

ExecptionDuplicateElement::ExecptionDuplicateElement(const std::string &argument)
    : _message("Error: Duplicate element in the config file: " + argument)
{
}

ExecptionDuplicateElement::~ExecptionDuplicateElement() throw()
{
}

const char *ExecptionDuplicateElement::what() const throw()
{
    return _message.c_str();
}

ExecptionMissElement::ExecptionMissElement(const std::string &argument)
    : _message("Error: Miss this mandatory part in the config file: " + argument)
{
}
ExecptionMissElement::~ExecptionMissElement() throw()
{
}
const char *ExecptionMissElement::what() const throw()
{
    return _message.c_str();
}

ExecptionErrorFunction::ExecptionErrorFunction(const std::string &argument)
    : _message("Error: This function has failed: " + argument)
{
}
ExecptionErrorFunction::~ExecptionErrorFunction() throw()
{
}
const char *ExecptionErrorFunction::what() const throw()
{
    return _message.c_str();
}

ExecptionErrorUninitializedVariable::ExecptionErrorUninitializedVariable(const std::string &var,
                                                                         const std::string &instance)
    : _message("Error: Attempt to use an uninitialized variable '" + var + "' in instance '" + instance + "'.")
{
}

ExecptionErrorUninitializedVariable::~ExecptionErrorUninitializedVariable() throw()
{
}

const char *ExecptionErrorUninitializedVariable::what() const throw()
{
    return _message.c_str();
}
