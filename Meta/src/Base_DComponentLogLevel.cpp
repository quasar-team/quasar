/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * Base_DComponentLogLevel.cpp
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
#include <boost/foreach.hpp>

#include <Configuration.hxx>
#include <Base_DComponentLogLevel.h>
#include <ASComponentLogLevel.h>

namespace Device
{

void Base_DComponentLogLevel::linkAddressSpace( AddressSpace::ASComponentLogLevel *addressSpaceLink)
{
    if (m_addressSpaceLink != 0)
    {
        /* signalize nice error from here. */
        abort();
    }
    else
        m_addressSpaceLink = addressSpaceLink;
}

//! Disconnects AddressSpace part from the Device logic, and does the same for all children
//! Returns number of unlinked objects including self
unsigned int Base_DComponentLogLevel::unlinkAllChildren ()
{
    unsigned int objectCounter=1;  // 1 is for self
    m_addressSpaceLink = 0;
    /* Fill up: call unlinkAllChildren on all children */

    return objectCounter;
}

Base_DComponentLogLevel::Base_DComponentLogLevel ()
:m_addressSpaceLink(0)
{}

Base_DComponentLogLevel::~Base_DComponentLogLevel ()
{}

std::string Base_DComponentLogLevel::getFullName() const
{
    if (m_addressSpaceLink)
    {
        return std::string(m_addressSpaceLink->nodeId().toFullString().toUtf8());
    }
    else
        return "-ObjectNotBoundToAddressSpace-";
}

}
