#ifdef BACKEND_OPEN62541

#ifndef MAIN_OPCSERVER_H
#define MAIN_OPCSERVER_H

#include <uaserver.h>

// forward-decls
namespace AddressSpace { class ASNodeManager; }

class OpcServer
{
    OpcServer(const OpcServer& other) = delete;
    OpcServer& operator= (const OpcServer& other) = delete;

public:
    // construction / destruction
    OpcServer();
    ~OpcServer();

    // Methods used to initialize the server
    int setServerConfig(const UaString& configurationFile, const UaString& applicationPath);

    int addNodeManager(AddressSpace::ASNodeManager* pNodeManager);

    /* This is just to create a certificate and quit right away */
    int createCertificate (
            const UaString& backendConfigFile,
            const UaString& appPath);

    // Methods used to start and stop the server
    int start();
    int stop(OpcUa_Int32 secondsTillShutdown, const UaLocalizedText& shutdownReason);

    // Access to default node manager
    NodeManagerConfig* getDefaultNodeManager();

    std::string getLogFilePath() { return m_logFilePath; }

private:
    std::string m_logFilePath;

    UaServer m_server;
};



#endif // MAIN_OPCSERVER_H

#endif // BACKEND_OPEN62541
