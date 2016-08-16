#ifndef BACKEND_OPEN62541
/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * QuasarServerCallback.cpp
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
#include "QuasarServerCallback.h"
#include "MetaUtils.h"
QuasarSession::QuasarSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken) : UaSession(sessionID, authenticationToken)
{
	MetaUtils::increaseSessionCounter();
}
QuasarSession::~QuasarSession()
{
	MetaUtils::decreaseSessionCounter();
}


QuasarServerCallback::QuasarServerCallback()
{
}

Session* QuasarServerCallback::createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken)
{
	return new QuasarSession(sessionID, authenticationToken);
}


 UaStatus QuasarServerCallback::logonSessionUser(Session* pSession, UaUserIdentityToken* pUserIdentityToken)
{
	 pSession->activate(0, pUserIdentityToken, *(pSession->getLocalIds()));
	return OpcUa_Good;
}

UaServer* QuasarServerCallback::createUaServer()
{
	return NULL;
}
#endif // BACKEND_OPEN62541
