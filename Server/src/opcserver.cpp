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
#include "opcserver.h"
#include "uamutex.h"
#include "srvtrace.h"

#include <version_coremodule.h>

#ifndef CPP_SDK_MAJOR
#define CPP_SDK_MAJOR PROD_MAJOR
#define CPP_SDK_MINOR PROD_MINOR
#define CPP_SDK_MINOR2 PROD_PATCH
#endif

#define UA_API_VERSION (CPP_SDK_MAJOR * 100 \
                               + CPP_SDK_MINOR * 10 \
                               + CPP_SDK_MINOR2 )

#if UA_API_VERSION < 140       // serverconfig comes from quasar
#include <serverconfigxml_quasar.h>
#else
#include "serverconfigxml.h"   // serverconfig comes from UA API
#endif


#include "uasession.h"
#include "uaunistring.h"
#include "coremodule.h"
#include "uamodule.h"
#include <list>
#include <iostream>
#include <LogIt.h>
#include <stdexcept>
#include <Utils.h>

using namespace std;

/** Class containing the private members for the OpcServer class. */
class OpcServerPrivate
{
public:
    OpcServerPrivate()
    : m_isStarted(OpcUa_False),
      m_pServerConfig(NULL),
      m_pServerManager(NULL),
      m_pCoreModule(NULL),
      m_pUaModule(NULL),
      m_pOpcServerCallback(NULL)
    {}
    ~OpcServerPrivate(){}

    OpcUa_Boolean      m_isStarted;
    UaMutex            m_mutex;
    UaString           m_configurationFile;
    UaString           m_applicationPath;
    ServerConfig*      m_pServerConfig;
    ServerManager*     m_pServerManager;
    CoreModule*        m_pCoreModule;
    UaModule*          m_pUaModule;
    OpcServerCallback* m_pOpcServerCallback;
    list<NodeManager*> m_listNodeManagers;
};


/** Basic server configuration class using the XML file format for internal use in the class OpcServer.*/
class ServerConfigBasicXml: public ServerConfigXml
{
public:
    ServerConfigBasicXml(const UaString& sXmlFileName, const UaString& sApplicationPath, OpcServerCallback* pOpcServerCallback);
    ~ServerConfigBasicXml(){}
    UaStatus afterLoadConfiguration(){return OpcUa_Good;}
    UaStatus startUp(ServerManager*){return OpcUa_Good;}
    UaStatus shutDown(){return OpcUa_Good;}
    Session* createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken);
    UaStatus logonSessionUser(Session* pSession, UaUserIdentityToken* pUserIdentityToken);
private:
    OpcServerCallback* m_pOpcServerCallback;
};


#define throw_runtime_error_with_origin(MSG) throw std::runtime_error(std::string("At ")+__FILE__+":"+Utils::toString(__LINE__)+" "+MSG)

/** Construction. */
OpcServer::OpcServer()
{
    d = new OpcServerPrivate;
    m_quasarCallback = new QuasarServerCallback();
    setCallback(m_quasarCallback);
}

/** Destruction. */
OpcServer::~OpcServer()
{
    if ( d->m_isStarted != OpcUa_False )
    {
        UaLocalizedText reason("en","Application shut down");
        stop(0, reason);
    }

    if ( d->m_pUaModule )
    {
        delete d->m_pUaModule;
        d->m_pUaModule = NULL;
    }

    if ( d->m_pCoreModule )
    {
        delete d->m_pCoreModule;
        d->m_pCoreModule = NULL;
    }

    // Delete all node managers
    list<NodeManager*>::iterator it;
    for ( it = d->m_listNodeManagers.begin(); it != d->m_listNodeManagers.end(); it++ )
    {
        delete (*it);
        (*it) = NULL;
    }
    d->m_listNodeManagers.clear();

    if ( d->m_pServerConfig )
    {
        delete d->m_pServerConfig;
        d->m_pServerConfig = NULL;
    }
    delete d;
    delete m_quasarCallback;
    d = NULL;
    m_quasarCallback = NULL;

    SrvT::closeTrace();
}

/** Sets the server configuration by passing the path of the configuration file.
 One of the overloaded methods needs to be called to give the server a valid configuration.<br> This version forces
 the server to use the default implementation for the ServerConfig object and allows to specify the configuraiton
 file and the path to the application or the directory containing the configuration and the PKI store..
 @param configurationFile Path and file name of the configuration file. The file can be one of the two file formats supported 
        by the SDK, a XML file format and an INI file format. The XML file format requires linking of the XML parser module and
        needs to be activated by compiling this class with the compiler switch SUPPORT_XML_CONFIG
 @param applicationPath The path of the configuration file and PKI store used to replace path placeholders in the configuration file
 @return Success code for the operation. Return 0 if the call succedded and -1 if the call failed. 
         This call fails if it is called after starting the server with the method start.
*/
int OpcServer::setServerConfig(const UaString& configurationFile, const UaString& applicationPath)
{
    UaMutexLocker lock(&d->m_mutex);
    if ( d->m_isStarted != OpcUa_False )
    {
        // Error, the method is called after the server was started
        return -1;
    }

    d->m_configurationFile = configurationFile;
    d->m_applicationPath = applicationPath;

    return 0;
}

/** Sets the server configuration by passing a server configuration object.
 One of the overloaded methods needs to be called to give the server a valid configuration.<br>
 This version allows to  pass in a ServerConfig object with a user specific implementation.
 @param pServerConfig Interface pointer of the object implementing the ServerConfig interface.
 @return Success code for the operation. Return 0 if the call succedded and -1 if the call failed. 
         This call fails if it is called after starting the server with the method start.
*/
int OpcServer::setServerConfig(ServerConfig* pServerConfig)
{
    UaMutexLocker lock(&d->m_mutex);
    if ( d->m_isStarted != OpcUa_False )
    {
        // Error, the method is called after the server was started
        return -1;
    }

    d->m_pServerConfig = pServerConfig;

    return 0;
}

/** Adds a node manager to the SDK.
 The node manager will be managed by this class including starting, stopping and deletion of the node manager.
 The method can be called several times for a list of node managers.
 If the method is called before start, all node managers will be started during the call of the start method. If this method is
 called if the server is already started the node manager will be started by this method.
 @return Success code for the operation. Return 0 if adding the node manager succedded and -1 if adding the node manager failed.
 */
int OpcServer::addNodeManager(NodeManager* pNodeManager)
{
    UaMutexLocker lock(&d->m_mutex);

    d->m_listNodeManagers.push_back(pNodeManager);

    if ( d->m_isStarted != OpcUa_False )
    {
        // Start up node manager if server is already started
        UaStatus ret = pNodeManager->startUp(d->m_pServerManager);
        if ( ret.isNotGood() )
        {
            TRACE1_ERROR(SERVER_UI, "Error: OpcServer::addNodeManager - can not start up node manager [ret=0x%lx]", ret.statusCode());
            return -1;
        }
    }

    return 0;
}

/** Sets the callback interface for the server object.
 This callback interface needs to be implemented if the application wants to implement user authentication.
 @param pOpcServerCallback Interface pointer of the callback interface.
 @return Success code for the operation. Return 0 if the call succedded and -1 if the call failed. 
         This call fails if it is called after starting the server with the method start.
*/
int OpcServer::setCallback(OpcServerCallback* pOpcServerCallback)
{
    UaMutexLocker lock(&d->m_mutex);
    if ( d->m_isStarted != OpcUa_False )
    {
        // Error, the method is called after the server was started
        return -1;
    }

    d->m_pOpcServerCallback = pOpcServerCallback;
    return 0;
}

/* Contribution from Slava Filimonov from OpcUaCanOpenServer,
 * Fix by Piotr Nikiel 08-Nov-2017 for UASDK 1.5.5 where few more miles have to be run.
 */
int OpcServer::createCertificate ()
{
    LOG(Log::INF) << "Will create certificate and exit.";
    UaMutexLocker lock(&d->m_mutex);
    int ret = 0;

    if ( d->m_isStarted != OpcUa_False )
    {
        LOG(Log::ERR) << " Error, the method is called after the server was already started";
        return -1;
    }

    // Create default configuration object if not provided by the application
    if ( d->m_pServerConfig == NULL )
    {
        UaUniString sConfigFile(d->m_configurationFile.toUtf8());
        sConfigFile = sConfigFile.toLower();

        if ( sConfigFile.lastIndexOf(".xml") > (sConfigFile.length() - 5) )
        {
            d->m_pServerConfig = new ServerConfigBasicXml(d->m_configurationFile, d->m_applicationPath, d->m_pOpcServerCallback);
        }

    }

    if ( d->m_pServerConfig == NULL )
    {
        LOG(Log::ERR) << "Problem opening server's backend configuration file -- check srvTrace logs";
        return -2;
    }

    // Check trace settings
    if (! d->m_pServerConfig->loadConfiguration().isGood() )
    {
        LOG(Log::ERR) << "Couldn't open server's backend configuration -- check srvTrace logs";
        return -3;
    }

    CoreModule coreModule;
    if( coreModule.initialize() != 0)
    {
        LOG(Log::ERR) << "CoreModule::initialize failed -- check srvTrace logs";
        return -4;
    }
    UaModule uaModule;
    UaServer *pUaServer = 0;
    if( uaModule.initialize(d->m_pServerConfig, pUaServer) != 0 )
    {
        LOG(Log::ERR) << "UaModule::initialize failed -- check srvTrace logs";
        return -5;
    }
    if( coreModule.startUp(d->m_pServerConfig) != 0)
    {
        LOG(Log::ERR) << "CoreModule::startUp failed -- check srvTrace logs";
        return -6;
    }
    if( uaModule.startUp(&coreModule) != 0)
    {
        LOG(Log::ERR) << "UaModule::startUp failed -- check srvTrace logs";
        return -7;
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
    UaMutexLocker lock(&d->m_mutex);
    int ret = 0;

    if ( d->m_isStarted != OpcUa_False )
        throw_runtime_error_with_origin("Error, the method is called after the server was already started");

    // Create default configuration object if not provided by the application
    if ( d->m_pServerConfig == NULL )
    {
        UaUniString sConfigFile(d->m_configurationFile.toUtf8());
        sConfigFile = sConfigFile.toLower();
        if ( sConfigFile.lastIndexOf(".ini") > (sConfigFile.length() - 5) )
        {
            throw std::runtime_error(".ini based configuration is not supported anymore.");
        }
        else if ( sConfigFile.lastIndexOf(".xml") > (sConfigFile.length() - 5) )
        {
            d->m_pServerConfig = new ServerConfigBasicXml(d->m_configurationFile, d->m_applicationPath, d->m_pOpcServerCallback);
        }
        else
        	throw_runtime_error_with_origin("Failed to understand format of Server Configuration. Config file not ending with .xml perhaps?");
    }

    if ( d->m_pServerConfig == NULL )
     	throw_runtime_error_with_origin("Failed to load server configuration file (typically called ServerConfig.xml or ServerConfig.ini) ");
 
    // Check trace settings
    if ( d->m_pServerConfig->loadConfiguration().isGood() )
    {

        OpcUa_Boolean bTraceEnabled    = OpcUa_False;
        OpcUa_UInt32  uTraceLevel      = 0;
        OpcUa_Boolean bSdkTraceEnabled = OpcUa_False;
        OpcUa_UInt32  uSdkTraceLevel   = 0;
        OpcUa_UInt32  uMaxTraceEntries = 0;
        OpcUa_UInt32  uMaxBackupFiles  = 0;
        UaString      sTraceFile;
		OpcUa_Boolean bDisableFlush    = OpcUa_False;

        d->m_pServerConfig->getStackTraceSettings( bTraceEnabled, uTraceLevel);


        
        
        d->m_pServerConfig->getServerTraceSettings(
            bSdkTraceEnabled,
            uSdkTraceLevel,
            uMaxTraceEntries,
            uMaxBackupFiles,
            sTraceFile
#if UA_API_VERSION >= 150
			,bDisableFlush
#endif
			);

        if ( bSdkTraceEnabled != OpcUa_False)
        {
	    m_logFilePath.assign( sTraceFile.toUtf8() );
            UaString sServerUri;
            UaLocalizedTextArray ltServerNames;
            d->m_pServerConfig->getServerInstanceInfo(sServerUri, ltServerNames);
            SrvT::initTrace( (UaTrace::TraceLevel)uSdkTraceLevel, uMaxTraceEntries, uMaxBackupFiles, sTraceFile, sServerUri);
            SrvT::setTraceActive(true);
            if ( bTraceEnabled != OpcUa_False)
            {
                SrvT::setStackTraceActive(OpcUa_True, uTraceLevel);
            }
        }
    }

    TRACE0_IFCALL(SERVER_UI, "==> OpcServer::start");

    // Create and initialize core server module
    d->m_pCoreModule = new CoreModule;
    ret = d->m_pCoreModule->initialize();
    if ( 0 != ret )
    {
        TRACE0_ERROR(SERVER_UI, "<== OpcServer::start - can not initialize core module");
	throw_runtime_error_with_origin("CoreModule::initialize returned "+Utils::toString(ret));
    }

    // Create and initialize UA server module
    d->m_pUaModule = new UaModule;
    // Check if we have a specialized implementation provided by the application
    UaServer *pUaServer = NULL;
    if (d->m_pOpcServerCallback)
    {
        pUaServer = d->m_pOpcServerCallback->createUaServer();
    }
    ret = d->m_pUaModule->initialize(d->m_pServerConfig,pUaServer);
    if ( 0 != ret )
    {
	throw_runtime_error_with_origin( "UaModule::initialize returned "+Utils::toString(ret) );
    }

    // Start core server module
    ret = d->m_pCoreModule->startUp(d->m_pServerConfig);
    if ( 0 != ret )
    {
        TRACE0_ERROR(SERVER_UI, "<== OpcServer::start - can not start up Core module");
	throw_runtime_error_with_origin( "CoreModule::startUp failed. Returned: "+Utils::toString(ret) );
      
    }
    else
    {
        UaStatus uaStatus;
        d->m_pServerManager = d->m_pCoreModule->getServerManager();
        uaStatus = d->m_pServerConfig->startUp(d->m_pServerManager);
        if ( uaStatus.isNotGood() )
        {
            TRACE1_ERROR(SERVER_UI, "<== OpcServer::start - can not start up Server Config [ret=0x%lx]", uaStatus.statusCode());
            d->m_pCoreModule->shutDown();
	    throw_runtime_error_with_origin( std::string("ServerConfig::startUp failed: ")+uaStatus.toString().toUtf8() );
	}
    }

    // Start NodeManagers
    list<NodeManager*>::iterator it;
    for ( it = d->m_listNodeManagers.begin(); it != d->m_listNodeManagers.end(); it++ )
    {
        // Start up node manager
        UaStatus ret = (*it)->startUp(d->m_pServerManager);
        if ( ret.isNotGood() )
        {
            TRACE1_ERROR(SERVER_UI, "Error: OpcServer::start - can not start up node manager [ret=0x%lx]", ret.statusCode());
	    throw_runtime_error_with_origin( std::string("NodeManager::startUp failed: ")+ret.toString().toUtf8() );
	}
    }

    // Start UA server module
    ret = d->m_pUaModule->startUp(d->m_pCoreModule);
    if ( 0 != ret )
    {

        TRACE0_ERROR(SERVER_UI, "<== OpcServer::start - can not start up UA module");

        d->m_pUaModule->shutDown();
        delete d->m_pUaModule;
        d->m_pUaModule = NULL;

        d->m_pCoreModule->shutDown();
        d->m_pServerManager = d->m_pCoreModule->getServerManager();
        delete d->m_pCoreModule;
        d->m_pCoreModule = NULL;

        d->m_pServerConfig->shutDown();
        delete d->m_pServerConfig;
        d->m_pServerConfig = NULL;

	
	throw_runtime_error_with_origin( std::string("UaModule::startUp failed: ")+Utils::toString(ret)+
					 " NOTE: UaModule startup fail typically means TCP/IP port in use or other network related issue. Refer to the logfile for details." );
 
    }

    UaString        sRejectedCertificateDirectory;
    OpcUa_UInt32 rejectedCertificateCount;
    UaEndpointArray uaEndpointArray;
    d->m_pServerConfig->getEndpointConfiguration(
        sRejectedCertificateDirectory,
	#if UA_API_VERSION >= 140
        rejectedCertificateCount,
        #endif
        uaEndpointArray);
    if ( uaEndpointArray.length() > 0 )
    {
        printf("***************************************************\n");
        printf(" Server opened endpoints for following URLs:\n");
        OpcUa_UInt32 idx;
        for ( idx=0; idx<uaEndpointArray.length(); idx++ )
        {
            printf("     %s\n", uaEndpointArray[idx]->sEndpointUrl().toUtf8());
        }
        printf("***************************************************\n");
    }

    d->m_isStarted = OpcUa_True;

    TRACE0_IFCALL(SERVER_UI, "<== OpcServer::start");
    return 0;
}

/** Stopps the OPC server
 Shutdown and deletes all SDK modules and NodeManagers.
 @param secondsTillShutdown Seconds till shutdown of the server if clients are connected.
 @param shutdownReason      Reason for the shutdown.
 @return Success code for the operation. Return 0 if the server stop succedded and -1 if the server stop failed.
 */
int OpcServer::stop(OpcUa_Int32 secondsTillShutdown, const UaLocalizedText& shutdownReason)
{
    TRACE0_IFCALL(SERVER_UI, "==> OpcServer::stop");

    UaMutexLocker lock(&d->m_mutex);
    if ( d->m_isStarted == OpcUa_False )
    {
        // Error, the server is not started
        return -1;
    }

    d->m_isStarted = OpcUa_False;

    // Check if we have active clients and sends shutdown information to clients
    // and wait the defined time if clients are connected to allow them to disconnect after they received the shutdown information
    OpcUa_UInt32 clientCount = d->m_pServerManager->startServerShutDown(secondsTillShutdown, shutdownReason);
    if ( clientCount > 0 )
    {
        UaThread::sleep(secondsTillShutdown);
    }

    // Stop UA server module
    if ( d->m_pUaModule )
    {
        d->m_pUaModule->shutDown();
        delete d->m_pUaModule;
        d->m_pUaModule = NULL;
    }

    // Stop core server module
    if ( d->m_pCoreModule )
    {
        d->m_pCoreModule->shutDown();
        delete d->m_pCoreModule;
        d->m_pCoreModule = NULL;
    }

    // Stop all node managers
    list<NodeManager*>::iterator it;
    for ( it = d->m_listNodeManagers.begin(); it != d->m_listNodeManagers.end(); it++ )
    {
        // Shut down node manager
        (*it)->shutDown();
        delete (*it);
        (*it) = NULL;
    }
    d->m_listNodeManagers.clear();

    // Stop server config
    if ( d->m_pServerConfig )
    {
        d->m_pServerConfig->shutDown();
        delete d->m_pServerConfig;
        d->m_pServerConfig = NULL;
    }

    TRACE0_IFCALL(SERVER_UI, "<== OpcServer::stop");
    SrvT::closeTrace();

    return 0;
}

/** Returns the default node manager for server specific nodes in namespace one.
 This node manager can be used to create objects and variables for data access. It can not be used for enhanced OPC UA features. Usind features
 like events, methods and historical access requires the implementation of a specific node manager.
 @return NodeManagerConfig interface used to add and delete UaNode objects in the node manager. Returns NULL if the server is not started.
 */
NodeManagerConfig* OpcServer::getDefaultNodeManager()
{
    UaMutexLocker lock(&d->m_mutex);
    if ( d->m_isStarted == OpcUa_False )
    {
        // Error, the server is not started
        return NULL;
    }

    return d->m_pServerManager->getNodeManagerNS1()->getNodeManagerConfig();
}



/** construction
 @param sXmlFileName Path and file name of the XML configuration file.
 @param sApplicationPath The path of the configuration file and PKI store used to replace path placeholders in the configuration file
 @param pOpcServerCallback Interface pointer of the callback interface.
 */
ServerConfigBasicXml::ServerConfigBasicXml(const UaString& sXmlFileName, const UaString& sApplicationPath, OpcServerCallback* pOpcServerCallback)
: ServerConfigXml(sXmlFileName, sApplicationPath
                  #if UA_API_VERSION >= 140
                  , /*config path*/ "",
                  /* trace path*/ "/tmp"
                  #endif
    ),
  m_pOpcServerCallback(pOpcServerCallback)
{}

/** Creates a session object for the OPC server.
 *  @param sessionID            Session Id created by the server application. 
 *  @param authenticationToken  Secret session Id created by the server application. 
 *  @return                     A pointer to the created session.
 */
Session* ServerConfigBasicXml::createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken)
{
    if ( m_pOpcServerCallback )
    {
        return m_pOpcServerCallback->createSession(sessionID, authenticationToken);
    }
    else
    {
        return new UaSession(sessionID, authenticationToken);
    }
}

/** Validates the user identity token and sets the user for a session.
 *  @param pSession             Interface to the Session context for the method call
 *  @param pUserIdentityToken   Secret session Id created by the server application. 
 *  @return                     Error code.
 */
UaStatus ServerConfigBasicXml::logonSessionUser(
    Session*             pSession,
    UaUserIdentityToken* pUserIdentityToken)
{
    OpcUa_Boolean  bEnableAnonymous;
    OpcUa_Boolean  bEnableUserPw;
    OpcUa_Boolean  bEnableCertificate;
    OpcUa_Boolean  bEnableKerberosTicket;

    // Get the settings for user identity tokens to support
    getUserIdentityTokenConfig(bEnableAnonymous, bEnableUserPw
#if UA_API_VERSION >= 140
                               , bEnableCertificate, bEnableKerberosTicket
#endif
        );

    if ( pUserIdentityToken->getTokenType() == OpcUa_UserTokenType_Anonymous )
    {
        if ( bEnableAnonymous == OpcUa_False )
        {
            return OpcUa_Bad;
        }
        else
        {
            return OpcUa_Good;
        }
    }
    else if ( pUserIdentityToken->getTokenType() == OpcUa_UserTokenType_UserName )
    {
        if ( bEnableUserPw == OpcUa_False )
        {
            return OpcUa_Bad;
        }
        else
        {
            if ( m_pOpcServerCallback )
            {
                return m_pOpcServerCallback->logonSessionUser(pSession, pUserIdentityToken);
            }
            else
            {
                return OpcUa_Bad;
            }
        }
    }

    return OpcUa_Bad;
}


#endif // BACKEND_UATOOLKIT
