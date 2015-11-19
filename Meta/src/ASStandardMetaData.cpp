/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * ASStandardMetaData.cpp
 *
 *  Created on: Aug 18, 2015
 *	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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
#include <iostream>




#include <ASStandardMetaData.h>


#include <DStandardMetaData.h>


namespace AddressSpace
{



/*ctr*/
ASStandardMetaData::ASStandardMetaData (
    UaNodeId parentNodeId,
    const UaNodeId& typeNodeId,
    ASNodeManager *nm,
    const Configuration::StandardMetaData & config):
    OpcUa::BaseObjectType (nm->makeChildNodeId(parentNodeId,"StandardMetaData"), "StandardMetaData", nm->getNameSpaceIndex(), nm),
    m_typeNodeId (typeNodeId)


    ,
    m_deviceLink (0)


{
    UaStatus s;
    UaVariant v;


}





ASStandardMetaData::~ASStandardMetaData ()
{

    if (m_deviceLink != 0)
    {
        PRINT("deviceLink not zero!!");
        PRINT_LOCATION();

    }

}




/* generate setters and getters (if requested) */





/* generate delegates (if requested) */





/* generate device logic link */


void ASStandardMetaData::linkDevice( Device::DStandardMetaData *deviceLink)
{
    if (m_deviceLink != 0)
    {
        /* This is an error -- device can be linked at most in the object's lifetime. */
        //TODO After error handling is discussed, abort in smarter way
        abort();

    }
    else
        m_deviceLink = deviceLink;
}


void ASStandardMetaData::unlinkDevice ()
{
    m_deviceLink = 0;
}


}

