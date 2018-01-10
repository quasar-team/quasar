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

using std::string;

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

bool Log::initializeDllLogging(LogItInstance* remoteInstance)
{
	return LogItInstance::setInstance(remoteInstance);
}

Log::LogComponentHandle Log::registerLoggingComponent(const string& componentName, const Log::LOG_LEVEL& nonComponentLogLevel /*=Log::INF*/)
{
    if(!LogItInstance::instanceExists()) return Log::INVALID_HANDLE;

    try
    {
    	const ComponentAttributes& componentAttributes = LogItInstance::getInstance()->registerLoggingComponent(componentName, nonComponentLogLevel);
    	return componentAttributes.getHandle();
    }
    catch(const std::runtime_error& e)
    {
    	LOG(Log::ERR) << __FUNCTION__ << " Failed to register logging component ["<<componentName<<"], message: "<<e.what();
    }

    return Log::INVALID_HANDLE;
}


bool Log::isLoggable(const Log::LOG_LEVEL& level)
{
    if(!LogItInstance::instanceExists()) return false;
    return level >= LogItInstance::getInstance()->m_nonComponentLogLevel;
}

bool Log::isLoggable(const Log::LOG_LEVEL& level, const LogComponentHandle& componentHandle)
{
	if(!LogItInstance::instanceExists()) return false;

    LOG_LEVEL componentLogLevel;
    if(!getComponentLogLevel(componentHandle, componentLogLevel)) return false; // unknown component handle.

    return level >= componentLogLevel;
}

bool Log::isLoggable(const Log::LOG_LEVEL& level, const std::string& componentName)
{
	if(!LogItInstance::instanceExists()) return false;

	const LogComponentHandle componentHandle = getComponentHandle(componentName);
	if(componentHandle == Log::INVALID_HANDLE) return false;

	return isLoggable(level, componentHandle);
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

const std::map<Log::LogComponentHandle, string> Log::getComponentLogsList()
{
	std::map<LogComponentHandle, string> result;
	if(!LogItInstance::instanceExists()) return result; // empty

	return LogItInstance::getInstance()->getLoggingComponentsList();
}

bool Log::setComponentLogLevel(const LogComponentHandle& componentHandle, const LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return false;

	ComponentAttributes* componentAttributes = nullptr;
	if(!LogItInstance::getInstance()->getComponentAttributes(componentHandle, componentAttributes)) return false;

    componentAttributes->setLevel(level);
    return true;
}

bool Log::getComponentLogLevel(const LogComponentHandle& componentHandle, LOG_LEVEL& level)
{
	if(!LogItInstance::instanceExists()) return false;

	ComponentAttributes* componentAttributes = nullptr;
	if(!LogItInstance::getInstance()->getComponentAttributes(componentHandle, componentAttributes)) return false;

    level = componentAttributes->getLevel();
    return true;
}

string Log::getComponentName(const LogComponentHandle& componentHandle)
{
	if(!LogItInstance::instanceExists()) return "UNKNOWN";

	ComponentAttributes* componentAttributes = nullptr;
	if(!LogItInstance::getInstance()->getComponentAttributes(componentHandle, componentAttributes)) return "UNKNOWN";

    return componentAttributes->getName();
}

Log::LogComponentHandle Log::getComponentHandle(const std::string componentName)
{
	if(!LogItInstance::instanceExists()) return Log::INVALID_HANDLE;
	LogComponentHandle componentHandle = Log::INVALID_HANDLE;
	if(!LogItInstance::getInstance()->getComponentHandle(componentName, componentHandle)) return Log::INVALID_HANDLE;
	return componentHandle;
}
