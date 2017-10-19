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
#include <boost/algorithm/string/case_conv.hpp>

#include "ComponentAttributes.h"
#include "LogSinks.h"
#include "LogLevels.h"
#include "LogItInstance.h"
#include "LogItInternalUtils.h"

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


bool Log::initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel)
{
	if(LogItInstance::instanceExists()) return false;
	LogItInstance::createInstance();
    initializeSinks();
    setNonComponentLogLevel(nonComponentLogLevel);
    return true;
}

uint64_t Log::addLogItComponent(const std::string& logComponentName, const Log::LOG_LEVEL& initialLogLevel /*= Log::INF*/)
{
	if(!LogItInstance::instanceExists())
	{
		// must call initializeLogging before adding components - throw exception
		std::ostringstream msg;
		msg << "ERROR: no logger instance found, failed to add logging component ["<<logComponentName<<"] probably logging is not initialized (call initializeLogging() before adding components)";
		throw std::runtime_error(msg.str());
	}

	ComponentAttributes componentAttributes(logComponentName, initialLogLevel);
	LogItInstance::getInstance()->addLogItComponent(componentAttributes);

	return componentAttributes.getId();
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

bool Log::isLoggable(const Log::LOG_LEVEL& level, const uint64_t& componentId)
{
	if(!LogItInstance::instanceExists()) return false;

    LOG_LEVEL componentLogLevel;
    if(!getComponentLogLevel(componentId, componentLogLevel)) return false; // unregistered component id.

    return level >= componentLogLevel;
}

bool Log::isLoggable(const Log::LOG_LEVEL& level, const std::string& componentId)
{
	return isLoggable(level, to64BitKey(componentId));
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

	const LogItInstance::ComponentMapPtr componentsMap = LogItInstance::getInstance()->getComponentMap();
	for(LogItInstance::ComponentMap::const_iterator it = componentsMap->begin(); it != componentsMap->end(); ++it)
	{
		result.push_back(it->second);
	}
	return result;
}

bool Log::setComponentLogLevel(const uint64_t& componentId, const LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return false;

	LogItInstance::ComponentMapPtr componentsMap = LogItInstance::getInstance()->getComponentMap();
	LogItInstance::ComponentMap::iterator pos = componentsMap->find(componentId);

	if(pos == componentsMap->end()) return false;

    pos->second.setLevel(level);
    return true;
}

bool Log::setComponentLogLevel(const std::string& componentId, const LOG_LEVEL& level)
{
	return setComponentLogLevel(to64BitKey(componentId), level);
}

bool Log::getComponentLogLevel(const uint64_t& componentId, LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return false;

	const LogItInstance::ComponentMapPtr componentsMap = LogItInstance::getInstance()->getComponentMap();
	LogItInstance::ComponentMap::const_iterator pos = componentsMap->find(componentId);

    if(pos == componentsMap->end()) return false;

    level = pos->second.getLevel();
    return true;
}

bool Log::getComponentLogLevel(const std::string& componentId, LOG_LEVEL& level)
{
	return getComponentLogLevel(to64BitKey(componentId), level);
}

void Log::setGlobalLogLevel(const LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return;

	LogItInstance::getInstance()->m_nonComponentLogLevel = level;

	LogItInstance::ComponentMapPtr componentsMap = LogItInstance::getInstance()->getComponentMap();
	for(LogItInstance::ComponentMap::iterator it = componentsMap->begin(); it != componentsMap->end(); ++it)
	{
		it->second.setLevel(level);
	}
}

std::string Log::componentIdToString(const uint64_t& componentId)
{
	if(!LogItInstance::instanceExists()) return "UNKNOWN";

	const LogItInstance::ComponentMapPtr componentsMap = LogItInstance::getInstance()->getComponentMap();
	LogItInstance::ComponentMap::const_iterator pos = componentsMap->find(componentId);

    if(pos == componentsMap->end()) return "UNKNOWN";

    return pos->second.getName();
}
