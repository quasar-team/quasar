/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * Base_DGeneralLogLevel.h
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
#ifndef __Base_DGeneralLogLevel__H__
#define __Base_DGeneralLogLevel__H__

#include <vector>
#include <boost/thread/mutex.hpp>

#include <statuscode.h>
#include <uadatetime.h>
#include <session.h>


#include <DRoot.h>
#include <Configuration.hxx>

#include <Base_DGeneralLogLevel.h>

/* forward decl for AddressSpace */
namespace AddressSpace {
class ASGeneralLogLevel;
}

namespace Device
{

/* forward declarations (comes from design.class.hasObjects) */


class
    Base_DGeneralLogLevel
{

public:
    /* Constructor */
    explicit Base_DGeneralLogLevel ();

private:
    /* No copy constructors or assignment operators */
    Base_DGeneralLogLevel (const Base_DGeneralLogLevel & other);
    Base_DGeneralLogLevel & operator=(const Base_DGeneralLogLevel & other);

public:

    /* sample dtr */
    virtual ~Base_DGeneralLogLevel ();


    /* add/remove for handling device structure */


    /* to safely quit */
    unsigned int unlinkAllChildren ();


    void linkAddressSpace (AddressSpace::
                           ASGeneralLogLevel
                           * as);
    AddressSpace::ASGeneralLogLevel * getAddressSpaceLink () const {
        return m_addressSpaceLink;
    }


    /* find methods for children */


    /* getters for values which are keys */



    /* mutex operations */


    /* variable-wise locks */


    /* query address-space for full name (mostly for debug purposes) */
    std::string getFullName() const;

private:

public:	  // TODO: reconsider this!
    AddressSpace::
    ASGeneralLogLevel
    *m_addressSpaceLink;

private:

    /* if any of our cachevariables has isKey=true then we shall keep its copy here for find functions  (it is const, either way) */


    /* if any of our config entries has isKey=true then we shall keep its copy here for find functions  (it is const, either way) */


    /* object-wise lock */


    /* variable-wise locks */





};





}

#endif // include guard
