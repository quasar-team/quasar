/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * ChangeNotifyingVariable.h
 *
 *  Created on: 12 Nov 2018
 *      Author: pnikiel
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

#include <ChangeNotifyingVariable.h>
#include <LogIt.h>

namespace AddressSpace
{

ChangeNotifyingVariable::ChangeNotifyingVariable(
    const UaNodeId&    nodeId,
    const UaString&    name,
    OpcUa_UInt16       browseNameNameSpaceIndex,
    const UaVariant&   initialValue,
    OpcUa_Byte         accessLevel,
    NodeManagerConfig* pNodeConfig,
    UaMutexRefCounted* pSharedMutex) :
    OpcUa::BaseDataVariableType(
        nodeId,
        name,
        browseNameNameSpaceIndex,
        initialValue,
        accessLevel,
        pNodeConfig,
        pSharedMutex)
{

}

ChangeNotifyingVariable::~ChangeNotifyingVariable()
{
}

UaStatus ChangeNotifyingVariable::setValue(
    Session* pSession,
    const UaDataValue& dataValue,
    OpcUa_Boolean checkAccessLevel)
{
    UaStatus status = OpcUa::BaseDataVariableType::setValue(pSession, dataValue, checkAccessLevel);
    if (status.isGood())
        for (OnChangeListener& changeListener : m_changeListeners)
        {
            changeListener(*this, dataValue);
        }
    return status;
}

void ChangeNotifyingVariable::addChangeListener (OnChangeListener onChangeListener)
{
    m_changeListeners.push_back(onChangeListener);
}

}


