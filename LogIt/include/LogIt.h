/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogIt.h
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
#ifndef LOGIT_H_
#define LOGIT_H_

#include <stdint.h>
#include <iostream>
#include <list>

#include "LogItStaticDefinitions.h"
#include "LogRecord.h"
#include "LogLevels.h"
#include "LogItInstance.h"

// internal macro - will be called by LOG(LOG_LEVEL)
#define LOG_WITHOUT_COMPONENT(level) \
     if( !Log::isLoggable(level) ) (void)0; \
     else LogRecord(__FILE__, __LINE__, level).getStream()

// internal macro - will be called by LOG(LOG_LEVEL, USER_COMPONENT)
#define LOG_WITH_COMPONENT(level, component) \
    if( !Log::isLoggable(level, component) ) (void)0; \
    else LogRecord(__FILE__, __LINE__, level, component).getStream()

/**
 * The one and only LOG macro - application code should log using this macro.
 * Invocations resolve to either
 *
 * - single arg 'LOG(LOG_LEVEL)' - LOG using the general purpose logging component.
 * or
 * - double arg 'LOG(LOG_LEVEL, component)' - LOG using a specific component (handle or name).
 *
 */
#define GET_LOG_MACRO(_1, _2, LOG_MACRO_NAME, ...) LOG_MACRO_NAME
#define LOG(...) GET_LOG_MACRO(__VA_ARGS__, LOG_WITH_COMPONENT, LOG_WITHOUT_COMPONENT)(__VA_ARGS__)

namespace Log
{
    /**
     * Initializers: Logging can only be initialized ONCE! To try and avoid thread contention. Note: although
     * logging is initialized once, logging thresholds (for non-component and component verbosity) can be set
     * at any time (by any thread).
     */

    /**
     * Simple initializer, without component specific logging.
     * Only
     * LOG(LOG_LEVEL)
     * invocations will be considered for logging, subject to logging verbosity threshold.
     */
    bool initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel = Log::INF);

	/**
	 * initializer to be called when using LogIt *inside* a shared library. The remoteLogInstance ptr should be supplied
	 * to the shared library from the main executable (note that this will probably require that the shared library API supports
	 * passing this ptr *before* the shared library can initialize the logger and start logging).
	 *
	 * The ptr (in the main executable, to pass to the DLL) is available from LogItInstance::getInstance() after the exe
	 * calls LogIt::initialiseLogging().
	 */
	bool initializeDllLogging(LogItInstance* remoteLogInstance);

    /**
     * register a user defined logging component.
     * LOG(LOG_LEVEL) invocations will be considered for logging
     * and
     * LOG(LOG_LEVEL, USER_COMPONENT) i.e. calls for registered components, will be considered
     * for logging, subject to threshold.
     *
     * RETURNS: the logging component handle, or, in case of error, Log::INVALID_HANDLE
     */
	LogComponentHandle registerLoggingComponent(const std::string& componentName, const Log::LOG_LEVEL& nonComponentLogLevel = Log::INF);

    /**
     * log check: should message be logged?
     *
     * no component (single-arg : level)
     * and
     * component specific (double-arg : level + (handle or name))
     */
    bool isLoggable(const Log::LOG_LEVEL& level);
    bool isLoggable(const Log::LOG_LEVEL& level, const LogComponentHandle& componentHandle);
    bool isLoggable(const Log::LOG_LEVEL& level, const std::string& componentName);

    /**
     * Get/Set the log threshold for all LOG messages without a component specified, i.e. calls such as
     * LOG(Log::INF) << "general log message"
     */
    void setNonComponentLogLevel(const Log::LOG_LEVEL& level);
    Log::LOG_LEVEL getNonComponentLogLevel(void);

    /**
     * Get/Set the log threshold for all LOG messages where a component is specified, i.e. calls such as
     * LOG(Log::INF, USER_DEFINED_COMPONENT_HANDLE_1) << "log message tagged with component id"
     *
     * Note getComponentLogLevel returns false if componentHandle not registered.
     */
    const std::map<Log::LogComponentHandle, std::string> getComponentLogsList();
    bool setComponentLogLevel(const LogComponentHandle& componentHandle, const LOG_LEVEL& level);
    bool getComponentLogLevel(const LogComponentHandle& componentHandle, LOG_LEVEL& level);

    /**
     * Finds the user defined string (specified in the registerLoggingComponent call)
     * for a given componentHandle.
     *
     * If there is no component registered with that handle, returns "UNKNOWN", otherwise
     * returns the component name.
     */
    std::string getComponentName(const LogComponentHandle& componentHandle);

    /**
     * Finds the component handle for a given component name.
     *
     * If there is no component registered with that name, returns Log::INVALID_HANDLE, otherwise
     * returns the component handle.
     */
    LogComponentHandle getComponentHandle(const std::string componentName);
}

#endif /* LOGIT_H_ */
