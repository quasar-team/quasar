#ifdef BACKEND_OPEN62541

#include <opcserver_open62541.h>

#include <LogIt.h>
#include <stdexcept>
#include <Utils.h>
#include <shutdown.h>

using namespace std;


#define throw_runtime_error_with_origin(MSG) throw std::runtime_error(std::string("At ")+__FILE__+":"+Utils::toString(__LINE__)+" "+MSG)

OpcServer::OpcServer():
    m_nodemanager(0),
    m_server_config(UA_ServerConfig_standard),
    m_server_network_layer(UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 4841)),
    m_server(0)
{
    //NOTE: UA_Server created later because it needs configuration (which is supplied later)
}

/** Destruction. */
OpcServer::~OpcServer()
{
    // UA_Server instance got deleted in stop()
}

int OpcServer::setServerConfig(const UaString& configurationFile, const UaString& applicationPath)
{
    LOG(Log::INF) << "Note: with open62541 backend, there isn't (yet) XML configuration loading. Assuming hardcoded server settings (endpoint's port, etc.)";
    // NOTE: some basid settings are configured in ctr init list
    // TODO: XML config reading
    m_server_config = UA_ServerConfig_standard;
    m_server_network_layer = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 4841);
    m_server_config.networkLayers = &m_server_network_layer;
    m_server_config.networkLayersSize = 1;
    return 0;
}

int OpcServer::addNodeManager(ASNodeManager* pNodeManager)
{
    if (!m_nodemanager)
	m_nodemanager = pNodeManager;
    else
	throw_runtime_error_with_origin("Sorry, only 1 NodeManager is supported.");
    return 0;
}

int OpcServer::createCertificate ()
{
    LOG(Log::ERR) << "Sorry, certificate creation is not supported(yet) with open62541 backend.";
    return -1;
}

int OpcServer::start()
{
    m_server = UA_Server_new(m_server_config);
    if (!m_server)
	throw_runtime_error_with_origin("UA_Server_new failed");
    m_nodemanager->linkServer(m_server);
    m_nodemanager->afterStartUp();
    m_open62541_server_thread = boost::thread ( &OpcServer::runThread, this );
    return 0;

}

int OpcServer::stop(OpcUa_Int32 secondsTillShutdown, const UaLocalizedText& shutdownReason)
{
    delete m_nodemanager;
    m_nodemanager = 0;
    UA_Server_delete(m_server);
    m_server = 0;
    return 0;
}


void OpcServer::runThread()
{
    UA_StatusCode retval = UA_Server_run(m_server, &g_RunningFlag);
}

#endif //  BACKEND_OPEN62541
