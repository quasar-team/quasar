/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
 * QuasarServerCallback.h
 *
 *  Created on: Nov 5, 2015
 * 		Author: Damian Abalo Miron <damian.abalo@cern.ch>
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

#ifndef _QUASARSERVERCALLBACK_H
#define _QUASARSERVERCALLBACK_H


#include "uasession.h"


class UaServer;
/** Callback interface for the main OPC Server object.
 This callback interface needs to be implemented if the application wants to implement user authentication.
 */
class OpcServerCallback
{
public:
    /** Construction */
    OpcServerCallback(){}
    /** Destruction */
    virtual ~OpcServerCallback(){}

    /** Creates a session object for the OPC server.
     This callback allows the application to create its own session class derived from UaSession to store
     user specific information and to implement the user logon and user verfication.
     @param sessionID            Session Id created by the server application.
     @param authenticationToken  Secret session Id created by the server application.
     @return                     A pointer to the created session.
     */
    virtual Session* createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken) = 0;

    /** Validates the user identity token and sets the user for the session.
     *  @param pSession             Interface to the Session context for the method call
     *  @param pUserIdentityToken   Secret session Id created by the server application.
     *  @return                     Error code.
     */
    virtual UaStatus logonSessionUser(Session* pSession, UaUserIdentityToken* pUserIdentityToken) = 0;

    /** Optional method used to create an application specific UaServer object used as main entry point for the UA communication.
     *  The SDK creates an instance of the UaServer class as default implementation.
     *  @return UaServer object or NULL if the SDK should use the default implementation.
     */
	virtual UaServer* createUaServer() { return NULL; }
};

class QuasarSession : public UaSession
{
public:
	QuasarSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken);
	virtual ~QuasarSession();
};

class QuasarServerCallback : public OpcServerCallback
{
private:
public:
	/** Construction */
	QuasarServerCallback();
	/** Destruction */
	virtual ~QuasarServerCallback(){}
	/** Creates a session object for the OPC server.
	 This callback allows the application to create its own session class derived from UaSession to store
	 user specific information and to implement the user logon and user verfication.
	 @param sessionID            Session Id created by the server application.
	 @param authenticationToken  Secret session Id created by the server application.
	 @return                     A pointer to the created session.
	 */
	Session* createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken);

	/** Validates the user identity token and sets the user for the session.
	 *  @param pSession             Interface to the Session context for the method call
	 *  @param pUserIdentityToken   Secret session Id created by the server application.
	 *  @return                     Error code.
	 */
	 UaStatus logonSessionUser(Session* pSession, UaUserIdentityToken* pUserIdentityToken);

	/** Optional method used to create an application specific UaServer object used as main entry point for the UA communication.
	 *  The SDK creates an instance of the UaServer class as default implementation.
	 *  @return UaServer object or NULL if the SDK should use the default implementation.
	 */
	UaServer* createUaServer();
};


#endif // _QUASARSERVERCALLBACK_H
