/*
 * ChangeNotifyingVariable.h
 *
 *  Created on: 12 Nov 2018
 *      Author: pnikiel
 */

#ifndef ADDRESSSPACE_INCLUDE_CHANGENOTIFYINGVARIABLE_H_
#define ADDRESSSPACE_INCLUDE_CHANGENOTIFYINGVARIABLE_H_

#include <opcua_basedatavariabletype.h>
#include <functional>
#include <list>

namespace AddressSpace
{

class ChangeNotifyingVariable: public OpcUa::BaseDataVariableType
{
public:

    typedef std::function<void (ChangeNotifyingVariable& fromWhere, const UaDataValue& newValue)> OnChangeListener;

    ChangeNotifyingVariable(
        const UaNodeId&    nodeId,
        const UaString&    name,
        OpcUa_UInt16       browseNameNameSpaceIndex,
        const UaVariant&   initialValue,
        OpcUa_Byte         accessLevel,
        NodeManagerConfig* pNodeConfig,
        UaMutexRefCounted* pSharedMutex = NULL);

    virtual ~ChangeNotifyingVariable();

    virtual UaStatus setValue(
        Session* pSession,
        const UaDataValue& dataValue,
        OpcUa_Boolean checkAccessLevel);

    virtual void addChangeListener (OnChangeListener onChangeListener);

private:
    std::list<OnChangeListener> m_changeListeners;
};

}



#endif /* ADDRESSSPACE_INCLUDE_CHANGENOTIFYINGVARIABLE_H_ */
