/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * DGeneralLogLevel.cpp
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
#include <Configuration.hxx>
#include <DGeneralLogLevel.h>
#include <ASGeneralLogLevel.h>
#include <LogIt.h>
#include <LogLevels.h>

namespace Device
{
DGeneralLogLevel::DGeneralLogLevel (const std::string& logLevel)
:Base_DGeneralLogLevel()
{
	writeLogLevel(logLevel.c_str());
}

DGeneralLogLevel::~DGeneralLogLevel ()
{}

UaStatus DGeneralLogLevel::writeLogLevel ( const UaString & v)
{
    Log::LOG_LEVEL targetLevel;

    if(logLevelFromString(v.toUtf8(), targetLevel))
    {
        LOG(Log::INF) << "setting log level to ["<<Log::logLevelToString(targetLevel)<<"]";
        Log::setNonComponentLogLevel(targetLevel);

        return OpcUa_Good;
    }
	else
	{
		LOG(Log::WRN) << " failed to set general log level to level ["<<v.toUtf8()<<"], unrecognized log level string (use on of {ERR|WRN|INF|DBG|TRC})";
	}


    return OpcUa_BadInvalidArgument;
}

}
