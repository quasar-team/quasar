/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASLog.cpp
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
#include <iostream>
#include <ASLog.h>


namespace AddressSpace
{

ASLog::ASLog (UaNodeId parentNodeId, const UaNodeId& typeNodeId, ASNodeManager *nm, const Configuration::Log & config)
:OpcUa::BaseObjectType (nm->makeChildNodeId(parentNodeId, "Log"), "Log", nm->getNameSpaceIndex(), nm),
 m_typeNodeId (typeNodeId)
{
    const UaStatus statusAddElementToParent = nm->addNodeAndReference( parentNodeId, this, OpcUaId_HasComponent);
    if (!statusAddElementToParent.isGood())
    {
        std::cout << "While addNodeAndReference from " << parentNodeId.toString().toUtf8() << " to " << this->nodeId().toString().toUtf8() << " : " << std::endl;
        ASSERT_GOOD(statusAddElementToParent);
    }
}

ASLog::~ASLog ()
{}

}

