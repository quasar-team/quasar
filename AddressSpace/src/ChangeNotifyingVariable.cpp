/*
 * ChangeNotifyingVariable.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: pnikiel
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


