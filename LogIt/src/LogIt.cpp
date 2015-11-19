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

#ifdef LOGIT_HAS_BOOSTLOG
#include "BoostRotatingFileLog.h"
#endif

#ifdef LOGIT_HAS_STDOUTLOG
#include <StdOutLog.h>
#endif

#ifdef LOGIT_HAS_UATRACE
#include <UaTraceSink.h>
#endif

bool g_sLoggingInitialized = false;
Log::LOG_LEVEL g_sNonComponentLogLevel(Log::INF);
std::map<uint32_t, ComponentAttributes> g_sComponents;

// internal - not exposed via API
void initializeSinks()
{
    LogSinks::g_sLogSinksInstance = new LogSinks();

	#ifdef LOGIT_HAS_BOOSTLOG
    BoostRotatingFileLog* fileLogger = new BoostRotatingFileLog();
    fileLogger->initialize();
    LogSinks::g_sLogSinksInstance->addSink(fileLogger);
	#endif // LOGIT_HAS_BOOSTLOG

	#ifdef LOGIT_HAS_STDOUTLOG
    StdOutLog * stdOutLog = new StdOutLog();
    stdOutLog->initialize();
    LogSinks::g_sLogSinksInstance->addSink(stdOutLog);
	#endif

	#ifdef LOGIT_HAS_UATRACE
    UaTraceSink * uaTraceSink = new UaTraceSink ();
    uaTraceSink->initialize();
    LogSinks::g_sLogSinksInstance->addSink(uaTraceSink);
    #endif
}

// internal - not exposed via API.
void registerComponents(const std::list<ComponentAttributes>& components)
{
    for(std::list<ComponentAttributes>::const_iterator it = components.begin(); it != components.end(); ++it)
    {
        ComponentAttributes component = *it;
        g_sComponents.insert( std::map<uint32_t, ComponentAttributes>::value_type(component.getId(), component) );
    }
}

bool Log::initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel)
{
    if(g_sLoggingInitialized) return false;

    initializeSinks();

    setNonComponentLogLevel(nonComponentLogLevel);
    g_sLoggingInitialized = true;
    return true;
}

bool Log::initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel, const std::list<ComponentAttributes>& components)
{
    if(g_sLoggingInitialized) return false;

    initializeSinks();

    registerComponents(components);
    setNonComponentLogLevel(nonComponentLogLevel);

    g_sLoggingInitialized = true;
    return true;
}

bool Log::isLoggable(const Log::LOG_LEVEL& level)
{
    if(!g_sLoggingInitialized) return false;
    return level >= g_sNonComponentLogLevel;
}

bool Log::isLoggable(const Log::LOG_LEVEL& level, const uint32_t& componentId)
{
    if(!g_sLoggingInitialized) return false;

    LOG_LEVEL componentLogLevel;
    if(!getComponentLogLevel(componentId, componentLogLevel)) return false; // unregistered component id.

    return level >= componentLogLevel;
}

void Log::setNonComponentLogLevel(const LOG_LEVEL& level)
{
    if(g_sNonComponentLogLevel != level)
    {
        g_sNonComponentLogLevel = level;
    }
}

Log::LOG_LEVEL Log::getNonComponentLogLevel(void)
{
    return g_sNonComponentLogLevel;
}

const std::list<ComponentAttributes> Log::getComponentLogsList()
{
	std::list<ComponentAttributes> result;
	for(std::map<uint32_t, ComponentAttributes>::const_iterator it = g_sComponents.begin(); it != g_sComponents.end(); ++it)
	{
		result.push_back(it->second);
	}
	return result;
}

bool Log::setComponentLogLevel(const uint32_t& componentId, const LOG_LEVEL& level)
{
    std::map<uint32_t, ComponentAttributes>::iterator pos = g_sComponents.find(componentId);
    if(pos == g_sComponents.end()) return false;

    pos->second.setLevel(level);
    return true;
}

bool Log::getComponentLogLevel(const uint32_t& componentId, LOG_LEVEL& level)
{
    std::map<uint32_t, ComponentAttributes>::const_iterator pos =  g_sComponents.find(componentId);
    if(pos == g_sComponents.end()) return false;

    level = pos->second.getLevel();
    return true;
}

void Log::setGlobalLogLevel(const LOG_LEVEL& level)
{
    g_sNonComponentLogLevel = level;
    for(std::map<uint32_t, ComponentAttributes>::iterator pos = g_sComponents.begin(); pos != g_sComponents.end(); ++pos)
    {
        pos->second.setLevel(level);
    }
}

std::string Log::componentIdToString(const uint32_t& componentId)
{
    std::map<uint32_t, ComponentAttributes>::iterator pos = g_sComponents.find(componentId);
    if(pos == g_sComponents.end()) return "UNKNOWN";

    return pos->second.getName();
}
