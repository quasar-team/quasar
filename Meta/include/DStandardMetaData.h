/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * DStandardMetaData.h
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
#ifndef __DStandardMetaData__H__
#define __DStandardMetaData__H__

#include <vector>
#include <boost/thread/mutex.hpp>

#include <statuscode.h>
#include <uadatetime.h>
#include <session.h>


#include <DRoot.h>
#include <Configuration.hxx>

#include <Base_DStandardMetaData.h>


namespace Device
{




class
    DStandardMetaData
    : public Base_DStandardMetaData
{

public:
    /* sample constructor */
    explicit DStandardMetaData ( const Configuration::StandardMetaData & config

                                 , DRoot * parent

                               ) ;
    /* sample dtr */
    ~DStandardMetaData ();




    /* delegators for
    cachevariables and sourcevariables */


private:
    /* Delete copy constructor and assignment operator */
    DStandardMetaData( const DStandardMetaData & );
    DStandardMetaData& operator=(const DStandardMetaData &other);

    // ----------------------------------------------------------------------- *
    // -     CUSTOM CODE STARTS BELOW THIS COMMENT.                            *
    // -     Don't change this comment, otherwise merge tool may be troubled.  *
    // ----------------------------------------------------------------------- *

public:

private:


};





}

#endif // include guard
