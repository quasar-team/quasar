#ifdef BACKEND_UATOOLKIT

/******************************************************************************
** opcserver.cpp
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

** @author Unified Automation
** @author Piotr Nikiel <piotr@nikiel.info>
******************************************************************************/
#include <opcserver.h>
#include <srvtrace.h>
#include <version_coremodule.h>
#include <serverconfigxml.h>
#include <coremodule.h>
#include <uamodule.h>
#include <LogIt.h>
#include <Utils.h>

using namespace std;

//For future switch in SDK version:
#ifndef CPP_SDK_MAJOR
#define CPP_SDK_MAJOR PROD_MAJOR
#define CPP_SDK_MINOR PROD_MINOR
#define CPP_SDK_MINOR2 PROD_PATCH
#endif
#define UA_API_VERSION (CPP_SDK_MAJOR * 100 \
                               + CPP_SDK_MINOR * 10 \
                               + CPP_SDK_MINOR2 )

/** We use this class only for create-certificate mode. In such a run mode no client will be allowed to connect. */
class TrivialServerConfigXml: public ServerConfigXml
{
public:
    TrivialServerConfigXml(const UaString& sXmlFileName, const UaString& sApplicationPath):
        ServerConfigXml(
                sXmlFileName,
                sApplicationPath,
                sApplicationPath,
                sApplicationPath) {};
    virtual ~TrivialServerConfigXml() {}
    UaStatus afterLoadConfiguration(){return OpcUa_Good;}
    UaStatus startUp(ServerManager*){return OpcUa_Good;}
    UaStatus shutDown(){return OpcUa_Good;}
    Session* createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken) { return new Session(sessionID, authenticationToken); }
    UaStatus logonSessionUser(Session* pSession, UaUserIdentityToken* pUserIdentityToken) { return OpcUa_Bad; }

};

/** Construction. */
OpcServer::OpcServer()
{
}

/** Destruction. */
OpcServer::~OpcServer()
{
}

/* Contribution from Slava Filimonov from OpcUaCanOpenServer,
 * Fix by Piotr Nikiel 08-Nov-2017 for UASDK 1.5.5 where few more miles have to be run.
 */
int OpcServer::createCertificate (
        const UaString& backendConfigFile,
        const UaString& appPath)
{
    SrvT::setTraceActive(true);
    SrvT::setPreFileTrace(true, UaTrace::ProgramFlow);

    LOG(Log::INF) << "Will create certificate and exit.";

    ServerConfig *serverConfig = new TrivialServerConfigXml(backendConfigFile, appPath);
    if (!serverConfig)
        throw_runtime_error_with_origin("XML backend config loader cant be instantiated");
    UaStatus status = serverConfig->loadConfiguration();
    if (!status.isGood())
        throw_runtime_error_with_origin("Backend config file cannot be loaded");

    OpcUa_Boolean bSdkTraceEnabled = OpcUa_False;
    OpcUa_UInt32  uSdkTraceLevel   = 0;
    OpcUa_UInt32  uMaxTraceEntries = 0;
    OpcUa_UInt32  uMaxBackupFiles  = 0;
    UaString      sTraceFile;
    OpcUa_Boolean bDisableFlush    = OpcUa_False;

    serverConfig->getServerTraceSettings(
            bSdkTraceEnabled,
            uSdkTraceLevel,
            uMaxTraceEntries,
            uMaxBackupFiles,
            sTraceFile,
            bDisableFlush);

    SrvT::initTrace( (UaTrace::TraceLevel)uSdkTraceLevel, uMaxTraceEntries, uMaxBackupFiles, sTraceFile, "");
    SrvT::setTraceActive(true);

    CoreModule coreModule;
    if( coreModule.initialize() != 0)
    {
        throw_runtime_error_with_origin("CoreModule::initialize failed -- check srvTrace logs");
    }

    if( coreModule.startUp(serverConfig) != 0)
    {
        throw_runtime_error_with_origin("CoreModule::startup failed -- check srvTrace logs");
    }

    UaModule uaModule;
    UaServer *pUaServer = 0;

    if( uaModule.initialize(serverConfig, pUaServer) != 0 )
    {
        throw_runtime_error_with_origin("UaModule::initialize failed -- check srvTrace logs");
    }

    if( uaModule.startUp(&coreModule) != 0)
    {
        throw_runtime_error_with_origin("UaModule::startUp failed -- check srvTrace logs");
    }

    uaModule.shutDown();
    coreModule.shutDown();
    return 0;
}


/** Starts the OPC server
 Initializes and starts up all NodeManagers and SDK modules.
 It is possible to add more NodeManagers after the server is started.
 @return Success code for the operation. Return 0 if the server start succedded and -1 if the server start failed.
 */
int OpcServer::start()
{

    UaTrace::setTraceHook(&m_quasarUaTraceHook);
    UaTrace::setPrintDateInTrace(true);

    LOG(Log::INF) << "Hook to UaTrace established";

    /*
     *  Optionally one can skip the extra log file and rely on quasar's output
     *  That can be done by adding the following line:
     *
     *  UaTrace::setSkipTraceAfterHook(true);
     *
     *  That is to be discussed, it has the pro to eliminate 1 out of the 3 log files of each server
     *  on the other hand it is a big change that should be considered
     */

    if (UaServerApplication::start() != 0)
    {
        std::list<UaString> errorList = SrvT::getPreFileTraces();
        if ( errorList.size() > 0 )
        {
            std::list<UaString>::iterator it;
            for ( it = errorList.begin(); it != errorList.end(); it++ )
            {
                LOG(Log::ERR) << it->toUtf8();
            }
        }
        else
        {
            LOG(Log::WRN) << "Check UaAppTraceFile log file for more details";
        }
        throw_runtime_error_with_origin("Failed to start-up the server.");
    }

    UaString sRejectedCertificateDirectory;
    OpcUa_UInt32 rejectedCertificateCount;
    UaEndpointArray uaEndpointArray;
    ServerConfig* serverConfig = getServerConfig();
    serverConfig->getEndpointConfiguration(
        sRejectedCertificateDirectory,
        rejectedCertificateCount,
        uaEndpointArray);

    for ( OpcUa_UInt32 idx=0; idx<uaEndpointArray.length(); idx++ )
        LOG(Log::INF) << "Opened endpoint: " << uaEndpointArray[idx]->sEndpointUrl().toUtf8();
    return 0;

}

#endif // BACKEND_UATOOLKIT
