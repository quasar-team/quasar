/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * BaseQuasarServer.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: Damian Abalo Miron <damian.abalo@cern.ch>
 *      Author: Piotr Nikiel <piotr@nikiel.info>
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <BaseQuasarServer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#ifndef BACKEND_OPEN62541
#include "uaplatformlayer.h"
#endif // BACKEND_OPEN62541

#include <boost/program_options.hpp>
#include "version.h"

#include <Configuration.hxx>
#include <Configurator.h>

#include <shutdown.h>
#include <LogIt.h>
#include <LogLevels.h>

#ifdef BACKEND_UATOOLKIT
   #include "xmldocument.h"
#endif

#include <shutdown.h>
#include <SourceVariables.h> // needed for starting up the SourceVariables threadpool
#include <meta.h>

#include <signal.h>
#ifdef __linux__
#include <unistd.h>
#include <pwd.h>
#elif _WIN32
#include <windows.h>
#include <Lmcons.h>
#include <direct.h>
#endif
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/join.hpp>

#include <MetaBuildInfo.h>
#include <CalculatedVariablesEngine.h>
#include <Utils.h>

#include <OpcuaToolkitInfo.hpp>

using namespace std;
using namespace boost::program_options;

BaseQuasarServer::BaseQuasarServer() :
        m_pServer(0),
        m_nodeManager(0)
{
}

BaseQuasarServer::~BaseQuasarServer()
{
    LOG(Log::TRC) << "Entered BaseQuasarServer dtr.";
    shutdownEnvironment();
}

int BaseQuasarServer::startApplication(int argc, char *argv[])
{

    bool isHelpOrVersionOrExtra = false;
    string configurationFileName = "config.xml";
    string opcUaBackendConfigFile; // default will come from parseCommandLine invocation
    bool isCreateCertificateOnly = false;

    int ret = parseCommandLine(argc, argv, &isHelpOrVersionOrExtra, &isCreateCertificateOnly, &configurationFileName,
            opcUaBackendConfigFile);

    if (ret != 0 || isHelpOrVersionOrExtra) //If there was a problem parsing the arguments, or it was a help/version call, we finish the execution
        return ret;
    try
    {
        ret = serverRun(configurationFileName, isCreateCertificateOnly, opcUaBackendConfigFile);
        LOG(Log::INF) << "OpcServerMain() exited with code [" << ret << "]";
        return ret;
    }
    catch (const std::exception &e)
    {
        LOG(Log::ERR) << "Caught exception with msg: [" << Quasar::TermColors::ForeRed() << e.what() << Quasar::TermColors::StyleReset() << "]";
        return 1;
    }
    catch (...)
    {
        LOG(Log::ERR) << Quasar::TermColors::ForeRed() << "Caught exception of *UNKNOWN* type in the server global scope " << Quasar::TermColors::StyleReset() <<
                "(and BTW have some mercy please and only throw subclasses of std::exception!)";
        return 1;
    }
}

int BaseQuasarServer::serverRun(
        const std::string& configFileName,
        bool onlyCreateCertificate,
        const std::string &opcUaBackendConfigurationFile)
{
    const std::string serverSettingsPath = getApplicationPath();
    const int initializeEnvironmentReturn = initializeEnvironment();
    if (initializeEnvironmentReturn != 0)
    {
        LOG(Log::ERR) << "Initialization of components failed. Return code: [" << initializeEnvironmentReturn
                << "]";
        return initializeEnvironmentReturn;
    }

    #ifndef BUILDING_SHARED_OBJECT
    // note from Piotr as per OPCUA-2355: shared objects will be loaded by some parent app so we shouldn't steal Ctrl-C from them.
    RegisterSignalHandler();
    #else
    LOG(Log::WRN) << "Not registering SIGINT handler because the target was built as a shared object";
    #endif // BUILDING_SHARED_OBJECT

    //- Start up OPC server ---------------------
    // This code can be integrated into a start up
    // sequence of the application where the
    // OPC server should be integrated
    //-------------------------------------------
    // Create and initialize server object

    m_pServer = new OpcServer;
    if (m_pServer->setServerConfig(opcUaBackendConfigurationFile.c_str(), serverSettingsPath.c_str()) != 0)
    {
        LOG(Log::ERR) << "setServerConfig() failed.";
        return -1;
    }

    if (onlyCreateCertificate)
    {
        LOG(Log::WRN) << Quasar::TermColors::ForeYellow() << "Note: the argument to create server certificate will be deprecated soon. Please stop using it." << Quasar::TermColors::StyleReset();
        return m_pServer->createCertificate(opcUaBackendConfigurationFile.c_str(), serverSettingsPath.c_str());
    }

    m_nodeManager = new AddressSpace::ASNodeManager();
    m_nodeManager->setAfterStartupDelegate(
            std::bind(&BaseQuasarServer::configurationInitializerHandler, this, configFileName, m_nodeManager));

    m_pServer->addNodeManager(m_nodeManager);

    int serverReturnCode = 0;

    try
    {
        const int startServerReturn = m_pServer->start();
        if (startServerReturn != 0)
        {
            serverStartFailLogError(startServerReturn, m_pServer->getLogFilePath());
            return startServerReturn;
        }

        mainLoop();
    }
    catch (const std::exception &e)
    {
        LOG(Log::ERR) << "Exception " <<
            Quasar::TermColors::ForeYellow() << typeid(e).name() << Quasar::TermColors::StyleReset() <<
            " caught in BaseQuasarServer::serverRun:  [" << Quasar::TermColors::ForeRed() << e.what() << Quasar::TermColors::StyleReset() << "]";
        serverReturnCode = 1;
    }
    AddressSpace::SourceVariables_destroySourceVariablesThreadPool ();
    shutdown();  // this is typically overridden by the developer

    unlinkAllDevices(m_nodeManager);
    Device::DRoot::getInstance()->unlinkAllChildren();

    m_pServer->stop(3, UaLocalizedText("", "User shut down"));
    if (m_pServer != 0)
    {
        delete m_pServer;
        m_pServer = NULL;
    }

    return serverReturnCode;
}

std::string BaseQuasarServer::getApplicationPath() const
{
#ifdef __linux__
    char serverSettingsPath[PATH_MAX];
    memset(serverSettingsPath, 0, sizeof serverSettingsPath);
    if (readlink("/proc/self/exe", serverSettingsPath, sizeof serverSettingsPath) < 0)
      throw std::runtime_error("Can't obtain self path -- /proc/self/exe not there?");
    char *pszFind = strrchr(serverSettingsPath, '/');
#elif _WIN32
    char serverSettingsPath[MAX_PATH];
    memset( serverSettingsPath, 0, sizeof serverSettingsPath );
    int bytes = GetModuleFileNameA(NULL, serverSettingsPath, MAX_PATH);
    char *pszFind = strrchr(serverSettingsPath, '\\');
#else
    char serverSettingsPath[0];  //We declare this so the compiler is happy
#error "ERROR: Unable to determine your platform."
#endif
    if (pszFind)
    {
        *pszFind = 0; // cut off appname
    }

    return std::string(serverSettingsPath);
}

std::string BaseQuasarServer::getProcessEnvironmentVariables() const
{
    std::ostringstream result;
#ifdef __linux__
    extern char **environ; // POSIX interface (https://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap08.html)
    char *s = *environ;
    for (int i = 0; s; ++i)
    {
        result << s << std::endl;
        s = *(environ + i);
    }
#elif _WIN32
    auto envBlockDeleterFn = [](LPTCH p) { FreeEnvironmentStrings(p); };
    auto envBlock = std::unique_ptr<TCHAR, decltype(envBlockDeleterFn)>{ GetEnvironmentStrings(), envBlockDeleterFn };
    // envBlock format: "keyA=valA\0keyB=valB\0...keyXXX=valXXX\0\0". Double \0\0 denotes end of block
    for (TCHAR* pos = envBlock.get(); pos != nullptr && *pos != TCHAR('\0'); )
    {
        const std::string envVar(pos); // capture from pos up to '\0'
        result << envVar << std::endl;
        pos += envVar.length() + 1; // +1 ? wind pos past '\0' at end of current key/val pair
    }
#endif
    return result.str();
}

/**
* Returns string in format: logged in user (effective process owner).
* Examples, where a user 'quasar' is logged in...
* == posix ==
* 'quasar' runs process regular - returns 'login:quasar uid:54321,quasar euid:54321,quasar'
* 'quasar' runs process with sudo - returns 'login:quasar uid:0,root euid:0,root'
* == windows ==
* 'quasar' runs process regular - returns 'login:quasar(priv:normal)'
* 'quasar' runs process with admin rights (or admin shell) 'login:quasar(priv:elevated)'
*/
std::string BaseQuasarServer::getProcessOwner() const
{
    std::ostringstream result;
#ifdef __linux__
    auto getUser = [](const uid_t& uid){
        const auto pswd = getpwuid(uid);
        const std::string name(pswd != nullptr && pswd->pw_name != nullptr ? pswd->pw_name : "unknown");
        std::ostringstream s;
        s << uid << "," << name;
        return s.str();
    };

    const auto userID = getlogin();
    result << "login:" << (userID != nullptr ? userID : "unknown") <<" uid:"<<getUser(getuid()) <<" euid:"<<getUser(geteuid());
#elif _WIN32
    char userID[UNLEN];
    memset(userID, 0, UNLEN);
    DWORD len = UNLEN;
    result << "login:" << (GetUserName(userID, &len) ? userID : "unknown");

    DWORD bytesUsed = 0;
    TOKEN_ELEVATION_TYPE elevationType = TokenElevationTypeDefault;
    std::string elevationString = "unknown";
    if (GetTokenInformation(GetCurrentProcessToken(), TokenElevationType, &elevationType, sizeof(elevationType), &bytesUsed))
    {
        elevationString = (elevationType == TokenElevationTypeFull ? "elevated" : "normal");
    }
    result << "(priv:" << elevationString << ")";
#endif
    return result.str();
}

int BaseQuasarServer::parseCommandLine(
        int argc,
        char *argv[],
        bool *isHelpOrVersionOrExtra,
        bool *isCreateCertificateOnly,
        std::string *configurationFileName,
        std::string& opcUaBackendConfigurationFile)
{
    for (int i = 0; i < argc; m_commandLineArgs.push_back(std::string(argv[i++]))) {};

    bool createCertificateOnly = false;
    bool printVersion = false;
    bool printVersionExtraInfo = false;
    string logFile;
    options_description desc("Allowed options");

    std::string defaultOpcUaBackendConfigurationFile = this->getApplicationPath() + "/ServerConfig.xml";

    desc.add_options()
            ("config_file", value<string>(), "A path to the config file")
            ("opcua_backend_config", value<string>(&opcUaBackendConfigurationFile)
             ->default_value(defaultOpcUaBackendConfigurationFile),
                 "(Optional) path to the OPC-UA settings file")
            ("create_certificate", bool_switch(&createCertificateOnly), "Create new certificate and exit")
            ("help,h", "Print help")
            ("version,v", bool_switch(&printVersion), "Print version and exit")
            ("version_extra", bool_switch(&printVersionExtraInfo), "Print version extra info and exit");

    positional_options_description p;
    p.add("config_file", 1);

    appendCustomCommandLineOptions(desc, p);

    variables_map vm;
    try
    {
    store(command_line_parser(argc,argv)
          .options(desc)
          .style(command_line_style::unix_style)
          .positional(p)
          .run(),
          vm);
    }
    catch (boost::exception &e)
    {
        cout << "Couldn't interpret command line, please run with --help " << endl;
        return 1;
    }
    notify(vm);
    if (vm.count("help"))
    {
        cout << desc << endl;
        if (isHelpOrVersionOrExtra)
            *isHelpOrVersionOrExtra = true;
        return 0;
    }

    //Print version if needed
    if (printVersion)
    {
        #ifdef BACKEND_UATOOLKIT
        std::string uasdkCoreModuleVersionInfo = VersionInfoCoreModule::getCoreModuleVersionInfo().toUtf8();
        #else
        std::string uasdkCoreModuleVersionInfo = "N/A in open62541 backend";
        #endif

        std::cout << VERSION_STR << std::endl << \
                Quasar::TermColors::ForeBold() << "\t BuildHost: " << Quasar::TermColors::StyleReset() << BuildMetaInfo::getBuildHost() << std::endl << \
                Quasar::TermColors::ForeBold() << "\t BuildTimestamp: " << Quasar::TermColors::StyleReset() << BuildMetaInfo::getBuildTime() << std::endl << \
                Quasar::TermColors::ForeBold() << "\t CommitID: " << Quasar::TermColors::StyleReset() << BuildMetaInfo::getCommitID() << std::endl << \
                Quasar::TermColors::ForeBold() << "\t ToolkitLibs: " << Quasar::TermColors::StyleReset() << BuildMetaInfo::getToolkitLibs() << std::endl << \
                Quasar::TermColors::ForeBold() << "\t BuildTooliktPath: " << Quasar::TermColors::StyleReset() << OpcuaToolkitInfo::getOpcuaToolkitPath() << std::endl;

        if (isHelpOrVersionOrExtra)
            *isHelpOrVersionOrExtra = true;
        return 0;
    }
    else if (printVersionExtraInfo)
    {
        auto stringWithoutSlashes = [](std::string& s) {
                                std::replace( s.begin(), s.end(), '\\', ' ') ;
                                return s;
                            };

        #ifdef BACKEND_UATOOLKIT
        std::string uasdkCoreModuleVersionInfo = VersionInfoCoreModule::getCoreModuleVersionInfo().toUtf8();
        std::string uaStackVersionInfo = VersionInfoCoreModule::getUaStackVersionInfo().toUtf8();
        std::string uaStackVersion = VersionInfoCoreModule::getUaStackVersion().toUtf8();
        std::string uaStackStaticConfigInfo = VersionInfoCoreModule::getUaStackStaticConfigInfo().toUtf8();
        std::string uaStackRuntimeConfigInfo = VersionInfoCoreModule::getUaStackRuntimeConfigInfo().toUtf8();
        std::string uaStackPlatformLayerVersionInfo = VersionInfoCoreModule::getUaStackPlatformLayerVersionInfo().toUtf8();
        std::string uaStackPlatformLayerVersion = VersionInfoCoreModule::getUaStackPlatformLayerVersion().toUtf8();
        std::string uaStackPlatformLayerConfigInfo = VersionInfoCoreModule::getUaStackPlatformLayerVersion().toUtf8();
        #else
        std::string uasdkCoreModuleVersionInfo = "N/A in open62541 backend";
        std::string uaStackVersionInfo = "N/A in open62541 backend";
        std::string uaStackVersion = "N/A in open62541 backend";
        std::string uaStackStaticConfigInfo = "N/A in open62541 backend";
        std::string uaStackRuntimeConfigInfo = "N/A in open62541 backend";
        std::string uaStackPlatformLayerVersionInfo = "N/A in open62541 backend";
        std::string uaStackPlatformLayerVersion = "N/A in open62541 backend";
        std::string uaStackPlatformLayerConfigInfo = "N/A in open62541 backend";
        #endif

        std::cout <<
                Quasar::TermColors::ForeBold() << "\t CoreModuleVersionInfo: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uasdkCoreModuleVersionInfo) << std::endl << \
                Quasar::TermColors::ForeBold() << "\t UaStackVersionInfo: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uaStackVersionInfo) << std::endl << \
                Quasar::TermColors::ForeBold() << "\t UaStackVersion: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uaStackVersion) << std::endl << \
                Quasar::TermColors::ForeBold() << "\t UaStackStaticConfigInfo: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uaStackStaticConfigInfo) << std::endl << \
                Quasar::TermColors::ForeBold() << "\t UaStackRuntimeConfigInfo: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uaStackRuntimeConfigInfo) << std::endl << \
                Quasar::TermColors::ForeBold() << "\t UaStackPlatformLayerVersionInfo: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uaStackPlatformLayerVersionInfo) << std::endl << \
                Quasar::TermColors::ForeBold() << "\t UaStackPlatformLayerVersion: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uaStackPlatformLayerVersion) << std::endl << \
                Quasar::TermColors::ForeBold() << "\t UaStackPlatformLayerConfigInfo: " << Quasar::TermColors::StyleReset() << stringWithoutSlashes(uaStackPlatformLayerConfigInfo) << std::endl;

        if (isHelpOrVersionOrExtra)
            *isHelpOrVersionOrExtra = true;
        return 0;
    }
    else
    {
        if (vm.count("config_file") > 0)
            *configurationFileName = vm["config_file"].as<string>();
        *isHelpOrVersionOrExtra = false;
        *isCreateCertificateOnly = createCertificateOnly;
        return 0;
    }
}
int BaseQuasarServer::initializeEnvironment()
{
#ifdef BACKEND_UATOOLKIT
    // initialize the XML Parser
    UaXmlDocument::initParser();

    // initialize the UA Stack platform layer
    const int ret = UaPlatformLayer::init();

#else
    const int ret = 0;
#endif
    initializeLogIt();
    logEnvironment();
    CalculatedVariables::Engine::initialize();
    return ret;
}
std::string BaseQuasarServer::getWorkingDirectory() const
{
    char* result = nullptr;
#ifdef __linux__
    char pathBuff[PATH_MAX];
    memset(pathBuff, 0, PATH_MAX);
    result = getcwd(pathBuff, PATH_MAX);
#elif _WIN32
    char pathBuff[MAX_PATH];
    memset(pathBuff, 0, MAX_PATH);
    result = _getcwd(pathBuff, MAX_PATH);
#endif
    return std::string(result != nullptr ? result : "unknown");
}
void BaseQuasarServer::logEnvironment() const
{
    LOG(Log::INF) << __FUNCTION__ << std::endl << \
        "\t Command line args: "<< boost::algorithm::join(m_commandLineArgs, " ") << std::endl << \
        "\t Current working directory: " << getWorkingDirectory() << std::endl << \
        "\t Directory of executable " << getApplicationPath() << std::endl << \
        "\t Process owner: " << getProcessOwner();
}
void BaseQuasarServer::initializeLogIt()
{
    Log::initializeLogging();
    Log::registerLoggingComponent("CalcVars", Log::INF);
    Log::registerLoggingComponent("AddressSpace", Log::INF);
    Log::registerLoggingComponent("ThreadPool", Log::INF);
}
void BaseQuasarServer::mainLoop()
{
    printServerMsg("Press " + std::string(SHUTDOWN_SEQUENCE) + " to shutdown server");

    // Wait for user command to terminate the server thread.

    while (ShutDownFlag() == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printServerMsg(" Shutting down server");
}
bool BaseQuasarServer::overridableConfigure(const std::string& fileName, AddressSpace::ASNodeManager *nm)
{
    return configure(fileName, nm);
}


void BaseQuasarServer::shutdownEnvironment()
{
#ifdef BACKEND_UATOOLKIT
    UaPlatformLayer::cleanup();
    UaXmlDocument::cleanupParser();
#endif
}

void BaseQuasarServer::serverStartFailLogError(int ret, const std::string& logFilePath)
{
    LOG(Log::ERR) << "Starting up of the server failed. Return code: [" << ret << "]";
    if (logFilePath != "")
        LOG(Log::ERR) << "The reason of failure should have been logged in your server log file: " << logFilePath;
    else
    {
    LOG(Log::ERR) << "The exact reason is unknown because you haven't enabled logging in your ServerConfig file.";
    LOG(Log::ERR) << "To enable, change value of <UaAppTraceEnabled> content to true.";
    }
}

void BaseQuasarServer::printServerMsg(const std::string& message)
{
    LOG(Log::INF) << "***************************************************";
    LOG(Log::INF) << message;
    LOG(Log::INF) << "***************************************************";
}

UaStatus BaseQuasarServer::configurationInitializerHandler(const std::string& configFileName,
        AddressSpace::ASNodeManager *nm)
{
    LOG(Log::INF) << "Configuration Initializer Handler";
    if (!overridableConfigure(configFileName, nm))
        return OpcUa_Bad; // error is already printed in configure()
    LOG(Log::DBG) << __FUNCTION__ << " Environment vars: " << std::endl << getProcessEnvironmentVariables();
    validateDeviceTree();
    Meta::initializeMeta(nm);
    CalculatedVariables::Engine::printInstantiationStatistics();
    CalculatedVariables::Engine::optimize();
    CalculatedVariables::Engine::setupSynchronization();
    CalculatedVariables::Engine::printInstantiationStatistics();
    initialize();
    return OpcUa_Good;
}

void BaseQuasarServer::appendCustomCommandLineOptions(options_description& commandLineOptions,
        positional_options_description& positionalOptionsDescription)
{
}
