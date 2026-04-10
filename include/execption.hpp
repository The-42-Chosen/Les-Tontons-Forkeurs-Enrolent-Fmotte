/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execption.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 13:08:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/10 14:43:41 by erpascua         ###   ########.fr       */
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