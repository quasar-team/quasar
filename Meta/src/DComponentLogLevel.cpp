/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * DComponentLogLevel.cpp
 *
 *  Created on: Aug 18, 2015
 * 		Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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

#include <Configuration.hxx>
#include <DComponentLogLevel.h>
#include <ASComponentLogLevel.h>
#include <LogIt.h>
#include <LogLevels.h>

namespace Device
{
DComponentLogLevel::DComponentLogLevel (const uint32_t& loggingComponentId, const std::string& logLevel)
:Base_DComponentLogLevel(), m_loggingComponentId(loggingComponentId), m_loggingComponentName(Log::componentIdToString(m_loggingComponentId))
{
	writeLogLevel(logLevel.c_str());
}

DComponentLogLevel::~DComponentLogLevel ()
{}

UaStatus DComponentLogLevel::writeLogLevel ( const UaString & v)
{
	Log::LOG_LEVEL targetLevel;
	if(Log::logLevelFromString(v.toUtf8(), targetLevel))
	{
		if(Log::setComponentLogLevel(m_loggingComponentId, targetLevel))
		{
			LOG(Log::INF) << "setting component [name:"<<m_loggingComponentName<<" id:"<<m_loggingComponentId<<"] to level ["<<Log::logLevelToString(targetLevel)<<"]";
			return OpcUa_Good;
		}
		else
		{
			LOG(Log::WRN) << "failed to set component [name:"<<m_loggingComponentName<<" id:"<<m_loggingComponentId<<"] to level ["<<Log::logLevelToString(targetLevel)<<"], most likely an invalid component id";
		}
	}
	else
	{
		LOG(Log::WRN) << " failed to set component [name:"<<m_loggingComponentName<<" id:"<<m_loggingComponentId<<"] to level ["<<v.toUtf8()<<"], unrecognized log level string (use on of {ERR|WRN|INF|DBG|TRC})";
	}

	return OpcUa_BadInvalidArgument;
}

}


