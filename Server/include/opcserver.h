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
#include "QuasarServerCallback.h"

class OpcServerPrivate;
class UaServer;

/** Main OPC Server object.
  This class is a utility class managing all server SDK modules for common use cases in a simplified way. 
  Enhanced users may replace this class with their own implementation to be able
  to use derived classes to overwrite SDK functionality.
  */
class OpcServer
{
    UA_DISABLE_COPY(OpcServer);
public:
    // construction / destruction
    OpcServer();
    ~OpcServer();

    // Methods used to initialize the server
    int setServerConfig(const UaString& configurationFile, const UaString& applicationPath);
    int setServerConfig(ServerConfig* pServerConfig);
    int addNodeManager(NodeManager* pNodeManager);
    int setCallback(OpcServerCallback* pOpcServerCallback);
    /* This is just to create a certificate and quit right away */
    int createCertificate ();

    // Methods used to start and stop the server
    int start();
    int stop(OpcUa_Int32 secondsTillShutdown, const UaLocalizedText& shutdownReason);

    // Access to default node manager
    NodeManagerConfig* getDefaultNodeManager();

    std::string getLogFilePath() { return m_logFilePath; }

private:
    OpcServerPrivate* d;
    std::string m_logFilePath;
    QuasarServerCallback* m_quasarCallback;
};



#endif // MAIN_OPCSERVER_H

#endif //  BACKEND_UATOOLKIT
