/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * BaseQuasarServer.h
 *
 *  Created on: Nov 6, 2015
 * 	Author: Damian Abalo Miron <damian.abalo@cern.ch>
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
 
#ifndef __BaseQuasarServer__H__
#define __BaseQuasarServer__H__
#include <string>
#include <uabase.h>
#include <uastring.h>//#include <opcserver.h>
#include <opcserver.h>
#include <ASNodeManager.h>
#include <DRoot.h>

/*
 * Class representing the internal logic for starting a Server up.
 * This class should not be modified by the final user. The class contain several virtual methods which represent the parts that will change depending on the implementation.
 * The final user should create a separate class inheriting from this one, and overriding the virual methods for properly customising the logic to fit the specific implementation.
 */
class BaseQuasarServer
{
public:
	//Constructor.
	BaseQuasarServer();
	virtual ~BaseQuasarServer();
	//Starts application. Parses command line arguments and then calls serverRun
	const int startApplication(int argc, char *argv[]);
protected:
	//Reference to the OPC UA Server internal implementation
	OpcServer* m_pServer;
	//Reference to the Node manager
	AddressSpace::ASNodeManager* m_nodeManager;
	/* Instantiate singleton for device collections */
	Device::DRoot root;

	//Main loop of the application logic.
	virtual void mainLoop();
	//Method for initialising Custom Modules, to be overwritten by the final user.
	virtual void initializeCustomModules(){}
	//Method for shutting down Custom Modules, to be overwritten by the final user
	//This method will be called from configurationInitializerHandler.
	virtual void shutdownCustomModules(){}
	//Method for initialising LogIt. Can be overriden for a specific implementation, but a default initialization is already provided.
	virtual void initializeLogIt();
	//Call to the method configure inside configuration. If a different configuration method wants to be implemented by the user, this should be overrided.
	//This method will be called from configurationInitializerHandler.
	virtual bool overridableConfigure(std::string fileName, AddressSpace::ASNodeManager *nm);
	//Gets the full path to the server configuration
	const UaString getServerConfigFullPath(const std::string szAppPath);
	//Gets the application path of the server
	const std::string getApplicationPath();
	//Logs the appropriate error when failing to start the server
	void serverStartFailLogError(const int ret, const std::string logFilePath);
	//Logs a server message
	void printServerMsg(const std::string);
private:
	//Starting point of the server, called by startApplication. Calls various other methods in the class, including mainLoop, which contains the mainLoop logic
	const int serverRun(const std::string configFileName, bool onlyCreateCertificate);
	//Parse command line arguments.
	const int parseCommandLine(int argc, char *argv[], bool *isHelpOrVersion, bool *isCreateCertificateOnly, std::string *configurationFileName);
	//initialize XML parser, logging and UA stack (in that order)
	const int initializeEnvironment();
	//Cleans the server before exiting.
	void shutdownEnvironment();
	//Handler for initializing the node manager configuration only when the server is ready
	UaStatus configurationInitializerHandler(std::string configFileName, AddressSpace::ASNodeManager *nm);

};
#endif // include guard
