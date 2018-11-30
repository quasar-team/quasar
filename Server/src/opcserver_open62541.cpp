#ifdef BACKEND_OPEN62541

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <opcserver_open62541.h>

#include <LogIt.h>
#include <stdexcept>
#include <Utils.h>
#include <shutdown.h>
#include <ServerConfig.hxx>

using namespace std;


// #define throw_runtime_error_with_origin(MSG) throw std::runtime_error(std::string("At ")+__FILE__+":"+Utils::toString(__LINE__)+" "+MSG)

OpcServer::OpcServer():
    m_nodemanager(0),
    #if UA_OPEN62541_VER_MINOR == 2
    m_server_config(UA_ServerConfig_standard),
    m_server_network_layer(UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 4841)),
    #else
    m_server_config(nullptr),
    #endif
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
    LOG(Log::WRN) << "Note: with open62541 backend, the ServerConfig loader is a new feature and most of your settings will be ignored. Please contribute to quasar.";
    std::auto_ptr< ::ServerConfig::OpcServerConfig > serverConfig;
    try
    {
        serverConfig = ServerConfig::OpcServerConfig_ (configurationFile.toUtf8());
    }
    catch (xsd::cxx::tree::parsing<char> &exception)
    {
        LOG(Log::ERR) << "ServerConfig loader: failed when trying to open the file, with general error message: " << exception.what();
        for( const xsd::cxx::tree::error<char> &error : exception.diagnostics() )
        {
            LOG(Log::ERR) << "ServerConfig: Problem at " << error.id() << ":" << error.line() << ": " << error.message();
        }
        throw std::runtime_error("ServerConfig: failed to load ServerConfig. The exact problem description should have been logged.");

    }

    // minimum one endpoint is guaranteed by the XSD, but in case user declared more, refuse to continue
    // TODO: implement multiple endpoints
    const ServerConfig::UaServerConfig& uaServerConfig = serverConfig->UaServerConfig();

    if (uaServerConfig.UaEndpoint().size() > 1)
    {
        throw_runtime_error_with_origin("No support for multiple UaEndpoint");
    }

    boost::regex endpointUrlRegex("^opc\\.tcp:\\/\\/\\[NodeName\\]:(?<port>\\d+)$");
    boost::smatch matchResults;
    std::string endpointUrl (uaServerConfig.UaEndpoint()[0].Url() );
    bool matched = boost::regex_match( endpointUrl, matchResults, endpointUrlRegex );
    if (!matched)
        throw_runtime_error_with_origin("Can't parse UaEndpoint/Url, note its should look like 'opc.tcp://[NodeName]:4841' perhaps with different port number, yours is '"+endpointUrl+"'");
    unsigned int endpointUrlPort = boost::lexical_cast<unsigned int>(matchResults["port"]);

    #if UA_OPEN62541_VER_MINOR == 2
    m_server_config = UA_ServerConfig_standard;
    m_server_network_layer = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, endpointUrlPort);
    m_server_config.networkLayers = &m_server_network_layer;
    m_server_config.networkLayersSize = 1;
    #else
    m_server_config = UA_ServerConfig_new_minimal(endpointUrlPort, /*certificate*/ nullptr);
    #endif


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
