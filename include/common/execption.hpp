/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execption.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 13:08:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/17 17:31:00 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class ExecptionMissSemiColon : public std::exception
{
  public:
    const char *what() const throw();
};

class ExecptionMissBrace : public std::exception
{
  public:
    const char *what() const throw();
};

class ExecptionWrongArgument : public std::exception
{
  private:
    std::string _message;

  public:
    ExecptionWrongArgument(const std::string &argument);
    ~ExecptionWrongArgument() throw();
    const char *what() const throw();
};

class ExecptionIllegalMethod : public std::exception
{
  private:
    std::string _message;

  public:
    ExecptionIllegalMethod(const std::string &argument);
    ~ExecptionIllegalMethod() throw();
    const char *what() const throw();
};

class ExecptionFailConvertion : public std::exception
{
  private:
    std::string _message;

  public:
    ExecptionFailConvertion(const std::string &argument);
    ~ExecptionFailConvertion() throw();
    const char *what() const throw();
};

class ExecptionMissElement : public std::exception
{
  private:
    std::string _message;

  public:
    ExecptionMissElement(const std::string &argument);
    ~ExecptionMissElement() throw();
    const char *what() const throw();
};

class ExecptionErrorFunction : public std::exception
{
  private:
    std::string _message;

  public:
    ExecptionErrorFunction(const std::string &argument);
    ~ExecptionErrorFunction() throw();
    const char *what() const throw();
};

class ExecptionErrorUninitializedVariable : public std::exception
{
  private:
    std::string _message;

  public:
    ExecptionErrorUninitializedVariable(const std::string &var, const std::string &instance);
    ~ExecptionErrorUninitializedVariable() throw();
    const char *what() const throw();
};