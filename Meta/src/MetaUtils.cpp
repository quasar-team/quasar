/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * MetaUtils.h
 *
 *  Created on: Aug 18, 2015
 * 		Author: Benjamin Farnham <benjamin.farnham@cern.ch>
 *		Author: Damian Abalo Miron <damian.abalo@cern.ch>
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
#include "MetaUtils.h"
unsigned int g_sessionCounter = 0;
Device::DServer* g_dServer = 0;
Device::DQuasar* g_dQuasar = 0;
Device::DSourceVariableThreadPool* g_dSourceVariableThreadPool = 0;

void MetaUtils::assertNodeAdded(const UaStatus& status, const UaNodeId& parentNodeId, const UaNodeId& childNodeId)
{
    if (!status.isGood())
    {
        std::cout << "While addNodeAndReference from " << parentNodeId.toString().toUtf8() << " to " << childNodeId.toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(status);
    }
}

Device::DServer* MetaUtils::getDServer( void ){	return( g_dServer );}
void MetaUtils::setDServer(Device::DServer* ser) {	g_dServer = ser; }

Device::DQuasar* MetaUtils::getDQuasar( void ){	return( g_dQuasar );}
void MetaUtils::setDQuasar(Device::DQuasar* ser) {	g_dQuasar = ser; }

Device::DSourceVariableThreadPool* MetaUtils::getDSourceVariableThreadPool( void ){	return( g_dSourceVariableThreadPool );}
void MetaUtils::setDSourceVariableThreadPool(Device::DSourceVariableThreadPool* ser) {	g_dSourceVariableThreadPool = ser; }


void MetaUtils::increaseSessionCounter()
{
	g_sessionCounter ++;
	if(g_dServer != 0)
	{
		g_dServer->setConnectedClientCount(g_sessionCounter);
	}
}
void MetaUtils::decreaseSessionCounter()
{
	g_sessionCounter --;
	if(g_dServer != 0)
	{
		g_dServer->setConnectedClientCount(g_sessionCounter);
	}
}
uint32_t MetaUtils::getSessionCounter()
{
	if( g_dServer != 0 )
	{
		g_sessionCounter = g_dServer->getConnectedClientCount();
		return( g_sessionCounter );
	} else {
		std::cout << __FILE__ << " " << __LINE__ << " MetaUtils::getSessionCounter: no server found" << std::endl;
	}
	return( 0 );
}



