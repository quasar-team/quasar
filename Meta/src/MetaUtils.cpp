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
#include "Certificate.h"

using std::string;

unsigned int g_sessionCounter = 0;
Device::DServer* g_dServer = 0;

void MetaUtils::assertNodeAdded(const UaStatus& status, const UaNodeId& parentNodeId, const UaNodeId& childNodeId)
{
    if (!status.isGood())
    {
        std::cout << "While addNodeAndReference from " << parentNodeId.toString().toUtf8() << " to " << childNodeId.toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(status);
    }
}


void MetaUtils::setDServer(Device::DServer* ser)
{
	g_dServer = ser;
}

void MetaUtils::increaseSessionCounter()
{
	g_sessionCounter ++;
	if(g_dServer != 0)
	{
		g_dServer->updateConnectedClientCount(g_sessionCounter);
	}
}

void MetaUtils::decreaseSessionCounter()
{
	g_sessionCounter --;
	if(g_dServer != 0)
	{
		g_dServer->updateConnectedClientCount(g_sessionCounter);
	}
}

string MetaUtils::calculateRemainingCertificateValidity(void)
{
    Certificate::Instance( Certificate::DEFAULT_PUBLIC_CERT_FILENAME, Certificate::DEFAULT_PRIVATE_CERT_FILENAME, Certificate::BEHAVIOR_TRY )->init();
    return Certificate::Instance()->remainingTime();
}



