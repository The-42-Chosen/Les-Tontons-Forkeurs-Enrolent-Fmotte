/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:35:38 by fmotte            #+#    #+#             */
/*   Updated: 2026/08/05 12:01:43 by erpascua         ###   ########.fr       */
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
    size_t _bodyBytesSent;
    EventData *_eventDataWriteChild;
    EventData *_eventDataReadChild;

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
    EventData *geteventDataWrite() const;
    void seteventDataWrite(EventData *_eventData);
    EventData *geteventDataRead() const;
    void seteventDataRead(EventData *_eventData);

    // =====================
    // == 	  Member	  ==
    // =====================
    void initializationCGIRequest(const std::string &interpreter);
    void createPipe(int pipeIn[2], int pipeOut[2]);

    void checkForkCreate(pid_t pid);
    void connectToEpoll();
    bool sendDataToChild();
    bool receivedDataFromChild();
    void closeStdinPipe();
    void closeStdoutPipe();
    void processDataFromChild();
    void forwardCgiHeaders(const std::string &headerBlock);
    void applyDefaultCgiStatus();
    void manage_pipe(const std::string &interpreter);
};