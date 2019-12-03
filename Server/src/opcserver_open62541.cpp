#ifdef BACKEND_OPEN62541

#include <opcserver_open62541.h>
#include <LogIt.h>

#include <ASNodeManager.h>
#include <shutdown.h>

OpcServer::OpcServer()
{
}

/** Destruction. */
OpcServer::~OpcServer()
{
}

int OpcServer::setServerConfig(const UaString& configurationFile, const UaString& applicationPath)
{
    m_server.setServerConfig( configurationFile, applicationPath );
    return 0;
}

int OpcServer::addNodeManager(AddressSpace::ASNodeManager* pNodeManager)
{
    m_server.addNodeManager(pNodeManager);
    return 0;
}

int OpcServer::createCertificate (
        const UaString& backendConfigFile,
        const UaString& appPath)
{
    LOG(Log::ERR) << "Sorry, certificate creation is not supported(yet) with open62541 backend.";
    return -1;
}

int OpcServer::start()
{
    m_server.linkRunningFlag(&g_RunningFlag);
    m_server.start();
    return 0;
}

int OpcServer::stop(OpcUa_Int32 secondsTillShutdown, const UaLocalizedText& shutdownReason)
{
    m_server.stop();
    return 0;
}


#endif //  BACKEND_OPEN62541
