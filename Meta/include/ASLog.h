/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASLog.h
 *
 *  Created on: Aug 18, 2015
 * 	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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

#ifndef __ASLog__H__
#define __ASLog__H__

#include <opcua_baseobjecttype.h>
#include <opcua_basedatavariabletype.h>

#include <Configuration.hxx>

#include <ASNodeManager.h>
#include <ASDelegatingVariable.h>
#include <ASSourceVariable.h>

namespace AddressSpace
{
class ASLog: public OpcUa::BaseObjectType
{
    UA_DISABLE_COPY(ASLog);

public:
    ASLog (UaNodeId parentNodeId, const UaNodeId& typeNodeId, ASNodeManager *nm, const Configuration::Log &config);
    virtual ~ASLog ();

    virtual UaNodeId typeDefinitionId () const {return m_typeNodeId;}

private:
    UaNodeId m_typeNodeId;
};

}

#endif // include guard

