#ifdef BACKEND_UATOOLKIT

/******************************************************************************
** opcserver.h
**
** Copyright (C) 2008-2009 Unified Automation GmbH. All Rights Reserved.
** Web: http://www.unified-automation.com
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Project: C++ OPC Server SDK sample code
**
** Description: Main OPC Server object class.
**
******************************************************************************/
#ifndef MAIN_OPCSERVER_H
#define MAIN_OPCSERVER_H

#include "serverconfig.h"
#include "nodemanager.h"
#include <string>
#include <uaserverapplication.h>
#include <QuasarUaTraceHook.hpp>

class OpcServer: public UaServerApplication
{
    UA_DISABLE_COPY(OpcServer);
public:
    // construction / destruction
    OpcServer();
    virtual ~OpcServer();

    /* This is just to create a certificate and quit right away */
    int createCertificate (
            const UaString& backendConfigFile,
            const UaString& appPath);

    // Methods used to start and stop the server
    int start();

    std::string getLogFilePath() { return m_logFilePath; }

private:
    std::string m_logFilePath;
    QuasarUaTraceHook m_quasarUaTraceHook;
};



#endif // MAIN_OPCSERVER_H

#endif //  BACKEND_UATOOLKIT
