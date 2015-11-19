/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * DComponentLogLevel.h
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
#ifndef __DComponentLogLevel__H__
#define __DComponentLogLevel__H__

#include <vector>
#include <stdint.h>
#include <boost/thread/mutex.hpp>

#include <statuscode.h>
#include <uadatetime.h>
#include <session.h>


#include <DRoot.h>
#include <Configuration.hxx>

#include <Base_DComponentLogLevel.h>
#include <LogIt.h>


namespace Device
{

class DComponentLogLevel : public Base_DComponentLogLevel
{
public:
    explicit DComponentLogLevel ( const uint32_t& loggingComponentId, const std::string& logLevel);
    ~DComponentLogLevel ();

    UaStatus writeLogLevel ( const UaString & v);


private:
    DComponentLogLevel( const DComponentLogLevel & );
    DComponentLogLevel& operator=(const DComponentLogLevel &other);

    const uint32_t m_loggingComponentId;
    const std::string m_loggingComponentName;
};

}

#endif // include guard
