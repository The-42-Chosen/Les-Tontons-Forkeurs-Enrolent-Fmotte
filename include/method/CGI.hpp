/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:35:38 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/18 13:14:09 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AMethod.hpp"
# include <unistd.h>

class CGI
{
    private:
        // =====================
        // ==    Attributs    ==
        // =====================
        AMethod *_amethod;
        int _pipeIn[2];
        int _pipeOut[2];
        pid_t _pid;
        
        CGI();
        
    public:
        // =====================
        // ==       OCF       ==
        // =====================
        CGI(AMethod *amethod);
        ~CGI();

        // =====================
        // ==     Getters     ==
        // =====================
        AMethod *getAmethod() const;
        void setAmethod(AMethod *amethod);
        int *getPipeIn();
        void setPipeIn(int pipeIn[2]);
        int* getPipeOut();
        void setPipeOut(int pipeOut[2]);
        pid_t getPid() const;
        void setPid(pid_t pid);
        
        // =====================
        // == 	  Member	  ==
        // =====================
        void initializationCGI(const std::string &path, const std::string &interpreter);
        void createPipe(int pipeIn[2], int pipeOut[2]);

        void checkForkCreate(pid_t pid);
        void connectToEpoll();
        
        void manage_pipe(const std::string &path, int pipeIn[2], int pipeOut[2], const std::string &interpreter);
};  

