/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogIt.cpp
 *
 *  Created on: Aug 18, 2015
 * 		Author: Benjamin Farnham <benjamin.farnham@cern.ch>
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

#include "LogIt.h"
#include <iostream>
#include <map>
#include <boost/algorithm/string/case_conv.hpp>

#include "ComponentAttributes.h"
#include "LogSinks.h"
#include "LogLevels.h"
#include "LogItInstance.h"

#ifdef LOGIT_HAS_BOOSTLOG
#include "BoostRotatingFileLog.h"
#endif

#ifdef LOGIT_HAS_STDOUTLOG
#include <StdOutLog.h>
#endif

#ifdef LOGIT_HAS_UATRACE
#include <UaTraceSink.h>
#endif

// internal - not exposed via API
void initializeSinks()
{
	if(!LogItInstance::instanceExists()) return;

	#ifdef LOGIT_HAS_BOOSTLOG
    BoostRotatingFileLog* fileLogger = new BoostRotatingFileLog();
    fileLogger->initialize();
    LogItInstance::getInstance()->m_logSinksInstance.addSink(fileLogger);
	#endif // LOGIT_HAS_BOOSTLOG

	#ifdef LOGIT_HAS_STDOUTLOG
    StdOutLog * stdOutLog = new StdOutLog();
    stdOutLog->initialize();
    LogItInstance::getInstance()->m_logSinksInstance.addSink(stdOutLog);
	#endif

	#ifdef LOGIT_HAS_UATRACE
    UaTraceSink * uaTraceSink = new UaTraceSink ();
    uaTraceSink->initialize();
    LogItInstance::getInstance()->m_logSinksInstance.addSink(uaTraceSink);
    #endif
}


// internal - not exposed via API.
void registerComponents(const std::list<ComponentAttributes>& components)
{
	if(!LogItInstance::instanceExists()) return;
    for(std::list<ComponentAttributes>::const_iterator it = components.begin(); it != components.end(); ++it)
    {
        ComponentAttributes component = *it;
        LogItInstance::getInstance()->m_sComponents.insert( std::map<uint32_t, ComponentAttributes>::value_type(component.getId(), component) );
    }
}


bool Log::initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel)
{
	if(LogItInstance::instanceExists()) return false;
	LogItInstance::createInstance();
    initializeSinks();
    setNonComponentLogLevel(nonComponentLogLevel);
    return true;
}

bool Log::initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel, const std::list<ComponentAttributes>& components)
{
	if(!initializeLogging(nonComponentLogLevel)) return false;
    registerComponents(components);
    return true;;
}

bool Log::initializeDllLogging(LogItInstance* remoteInstance)
{
	return LogItInstance::setInstance(remoteInstance);
}

bool Log::isLoggable(const Log::LOG_LEVEL& level)
{
    if(!LogItInstance::instanceExists()) return false;
    return level >= LogItInstance::getInstance()->m_nonComponentLogLevel;
}

bool Log::isLoggable(const Log::LOG_LEVEL& level, const uint32_t& componentId)
{
	if(!LogItInstance::instanceExists()) return false;

    LOG_LEVEL componentLogLevel;
    if(!getComponentLogLevel(componentId, componentLogLevel)) return false; // unregistered component id.

    return level >= componentLogLevel;
}

void Log::setNonComponentLogLevel(const LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return;
    if(LogItInstance::getInstance()->m_nonComponentLogLevel != level)
    {
    	LogItInstance::getInstance()->m_nonComponentLogLevel = level;
    }
}

Log::LOG_LEVEL Log::getNonComponentLogLevel(void)
{
	return LogItInstance::getInstance()->m_nonComponentLogLevel;
}

const std::list<ComponentAttributes> Log::getComponentLogsList()
{
	std::list<ComponentAttributes> result;
	if(!LogItInstance::instanceExists()) return result; // empty

	for(std::map<uint32_t, ComponentAttributes>::const_iterator it = LogItInstance::getInstance()->m_sComponents.begin(); it != LogItInstance::getInstance()->m_sComponents.end(); ++it)
	{
		result.push_back(it->second);
	}
	return result;
}

bool Log::setComponentLogLevel(const uint32_t& componentId, const LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return false;

    std::map<uint32_t, ComponentAttributes>::iterator pos = LogItInstance::getInstance()->m_sComponents.find(componentId);
    if(pos == LogItInstance::getInstance()->m_sComponents.end()) return false;

    pos->second.setLevel(level);
    return true;
}

bool Log::getComponentLogLevel(const uint32_t& componentId, LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return false;

	std::map<uint32_t, ComponentAttributes>::const_iterator pos = LogItInstance::getInstance()->m_sComponents.find(componentId);
    if(pos == LogItInstance::getInstance()->m_sComponents.end()) return false;

    level = pos->second.getLevel();
    return true;
}

void Log::setGlobalLogLevel(const LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return;

	LogItInstance::getInstance()->m_nonComponentLogLevel = level;
    for(std::map<uint32_t, ComponentAttributes>::iterator pos = LogItInstance::getInstance()->m_sComponents.begin(); pos != LogItInstance::getInstance()->m_sComponents.end(); ++pos)
    {
        pos->second.setLevel(level);
    }
}

std::string Log::componentIdToString(const uint32_t& componentId)
{
	if(!LogItInstance::instanceExists()) return "UNKNOWN";

    std::map<uint32_t, ComponentAttributes>::iterator pos = LogItInstance::getInstance()->m_sComponents.find(componentId);
    if(pos == LogItInstance::getInstance()->m_sComponents.end()) return "UNKNOWN";

    return pos->second.getName();
}
