/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * DGeneralLogLevel.h
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
#ifndef __DGeneralLogLevel__H__
#define __DGeneralLogLevel__H__

#include <vector>
#include <boost/thread/mutex.hpp>

#include <statuscode.h>
#include <uadatetime.h>
#include <session.h>


#include <DRoot.h>
#include <Configuration.hxx>

#include <Base_DGeneralLogLevel.h>


namespace Device
{




class
    DGeneralLogLevel
    : public Base_DGeneralLogLevel
{

public:
    explicit DGeneralLogLevel (const std::string& logLevel);
    ~DGeneralLogLevel ();

    /* delegators for
    cachevariables and sourcevariables */

    /* Note: never directly call this function. */


    UaStatus writeLogLevel ( const UaString & v);


private:
    /* Delete copy constructor and assignment operator */
    DGeneralLogLevel( const DGeneralLogLevel & );
    DGeneralLogLevel& operator=(const DGeneralLogLevel &other);

    // ----------------------------------------------------------------------- *
    // -     CUSTOM CODE STARTS BELOW THIS COMMENT.                            *
    // -     Don't change this comment, otherwise merge tool may be troubled.  *
    // ----------------------------------------------------------------------- *

public:

private:


};





}

#endif // include guard
