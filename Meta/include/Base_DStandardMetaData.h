/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * Base_DStandardMetaData.h
 *
 *  Created on: Aug 18, 2015
 * 	Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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
#ifndef __Base_DStandardMetaData__H__
#define __Base_DStandardMetaData__H__

#include <vector>
#include <boost/thread/mutex.hpp>

#include <statuscode.h>
#include <uadatetime.h>
#include <session.h>


#include <DRoot.h>
#include <Configuration.hxx>

#include <Base_DStandardMetaData.h>

/* forward decl for AddressSpace */
namespace AddressSpace {
class
    ASStandardMetaData
    ;
}
/* forward decl for Parent */

#include <DRoot.h>


namespace Device
{

class
    Base_DStandardMetaData
{

public:
    /* Constructor */
    explicit Base_DStandardMetaData ( const Configuration::StandardMetaData & config , DRoot * parent);

private:
    /* No copy constructors or assignment operators */
    Base_DStandardMetaData (const Base_DStandardMetaData & other);
    Base_DStandardMetaData & operator=(const Base_DStandardMetaData & other);

public:

    /* sample dtr */
    virtual ~Base_DStandardMetaData ();

    DRoot * getParent () const {return m_parent;}


    /* to safely quit */
    unsigned int unlinkAllChildren ();


    void linkAddressSpace (AddressSpace::ASStandardMetaData* as);
    AddressSpace::ASStandardMetaData * getAddressSpaceLink () const {return m_addressSpaceLink;}


    /* query address-space for full name (mostly for debug purposes) */
    std::string getFullName() const;

private:

    DRoot * m_parent;


public:	  // TODO: reconsider this!
    AddressSpace::ASStandardMetaData* m_addressSpaceLink;

};

}

#endif // include guard
