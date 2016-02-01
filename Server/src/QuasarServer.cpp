/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * QuasarServer.cpp
 *
 *  Created on: Nov 6, 2015
 * 		Author: Damian Abalo Miron <damian.abalo@cern.ch>
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

#include "QuasarServer.h"
#include <LogIt.h>
#include <string.h>
#include <shutdown.h>
#include <DRoot.h>
#include <boost/foreach.hpp>
#include <DClass.h>
#include <ASClass.h>
#include <stdlib.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

QuasarServer::QuasarServer() : BaseQuasarServer()
{

}

QuasarServer::~QuasarServer()
{
 
}

void QuasarServer::mainLoop()
{
    printServerMsg("Press "+std::string(SHUTDOWN_SEQUENCE)+" to shutdown server");

    // Wait for user command to terminate the server thread.

    while(!ShutDownFlag())
    {

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	BOOST_FOREACH(Device::DClass* cl, Device::DRoot::getInstance()->classs() )
	{
	    cl->getAddressSpaceLink()->setVarUInt32( rand(), OpcUa_Good );
	}

    }
    printServerMsg(" Shutting down server");
}

void QuasarServer::initialize()
{
    LOG(Log::INF) << "Initializing Quasar server.";

}

void QuasarServer::shutdown()
{
	LOG(Log::INF) << "Shutting down Quasar server.";
}

void QuasarServer::initializeLogIt()
{
	Log::initializeLogging();
  LOG(Log::INF) << "Logging initialized.";
}
