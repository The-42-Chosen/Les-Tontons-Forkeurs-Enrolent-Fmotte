/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:35:38 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/22 12:43:07 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ARequest.hpp"

class CGIRequest : public ARequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    int _pipeIn[2];
    int _pipeOut[2];
    pid_t _pid;
    std::string _cgiBuffer;
  
    CGIRequest();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    CGIRequest(ARequest arequest);
    ~CGIRequest();

    // =====================
    // ==     Getters     ==
    // =====================
    int *getPipeIn();
    void setPipeIn(int pipeIn[2]);
    int *getPipeOut();
    void setPipeOut(int pipeOut[2]);
    pid_t getPid() const;
    void setPid(pid_t pid);

    // =====================
    // == 	  Member	  ==
    // =====================
    void initializationCGIRequest(const std::string &interpreter);
    void createPipe(int pipeIn[2], int pipeOut[2]);

    void checkForkCreate(pid_t pid);
    void connectToEpoll();
    void sendDataToChild();
    bool receivedDataFromChild();
    void processDataFromChild();
    void forwardCgiHeaders(const std::string &headerBlock);
    void manage_pipe(const std::string &interpreter);
};