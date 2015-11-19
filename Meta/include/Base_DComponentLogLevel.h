/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * Base_DComponentLogLevel.h
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
#ifndef __Base_DComponentLogLevel__H__
#define __Base_DComponentLogLevel__H__

#include <vector>
#include <boost/thread/mutex.hpp>

#include <statuscode.h>
#include <uadatetime.h>
#include <session.h>


#include <DRoot.h>
#include <Configuration.hxx>

#include <Base_DComponentLogLevel.h>

/* forward decl for AddressSpace */
namespace AddressSpace {
class
    ASComponentLogLevel
    ;
}

namespace Device
{

/* forward declarations (comes from design.class.hasObjects) */


class Base_DComponentLogLevel
{

public:
    /* Constructor */
    explicit Base_DComponentLogLevel ();

private:
    /* No copy constructors or assignment operators */
    Base_DComponentLogLevel (const Base_DComponentLogLevel & other);
    Base_DComponentLogLevel & operator=(const Base_DComponentLogLevel & other);

public:

    virtual ~Base_DComponentLogLevel ();

    /* to safely quit */
    unsigned int unlinkAllChildren ();

    void linkAddressSpace (AddressSpace::ASComponentLogLevel* as);
    AddressSpace::ASComponentLogLevel * getAddressSpaceLink () const {return m_addressSpaceLink;}

    /* query address-space for full name (mostly for debug purposes) */
    std::string getFullName() const;

public:	  // TODO: reconsider this!
    AddressSpace::ASComponentLogLevel* m_addressSpaceLink;
};





}

#endif // include guard
