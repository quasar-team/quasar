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
    virtual size_t changeListenerSize () const { return m_changeListeners.size(); }
    virtual void removeAllChangeListeners () { m_changeListeners.clear(); }

private:
    std::list<OnChangeListener> m_changeListeners;
};

}



#endif /* ADDRESSSPACE_INCLUDE_CHANGENOTIFYINGVARIABLE_H_ */
