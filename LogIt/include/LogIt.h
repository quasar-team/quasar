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

#include "LogRecord.h"
#include "LogLevels.h"
#include "ComponentAttributes.h"
#include "LogItInstance.h"

const uint32_t g_sMaxComponentIdCount = 1024;

// internal macro - will be called by LOG(LOG_LEVEL)
#define LOG_WITHOUT_COMPONENT_ID(level) \
     if( !Log::isLoggable(level) ) (void)0; \
     else LogRecord(__FILE__, __LINE__, level).getStream()

// internal macro - will be called by LOG(LOG_LEVEL, USER_COMPONENT_ID)
#define LOG_WITH_COMPONENT_ID(level, component) \
    if( !Log::isLoggable(level, component) ) (void)0; \
    else LogRecord(__FILE__, __LINE__, level, component).getStream()

/**
 * The one and only LOG macro - application code should log using this macro.
 * Invocations resolve to either
 *
 * - single arg 'LOG(LOG_LEVEL)' - LOG without component ID.
 * or
 * - double arg 'LOG(LOG_LEVEL, componentId)' - LOG with a specific component ID.
 *
 */
#define GET_LOG_MACRO(_1, _2, LOG_MACRO_NAME, ...) LOG_MACRO_NAME
#define LOG(...) GET_LOG_MACRO(__VA_ARGS__, LOG_WITH_COMPONENT_ID, LOG_WITHOUT_COMPONENT_ID)(__VA_ARGS__)

namespace Log
{
    /**
     * Initializers: Logging can only be initialized ONCE! To try and avoid thread contention. Note: although
     * logging is initialized once, logging thresholds (for non-component and component verbosity) can be set
     * at any time (by any thread).
     */

    /**
     * Simple initializer, without component specific logging. Only
     * LOG(LOG_LEVEL)
     * invocations will be considered for logging,
     *
     * all LOG(LOG_LEVEL, USER_COMPONENT_ID) calls are ignored.
     */
    bool initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel = Log::INF);

    /**
     * initializer with user defined component support.
     * LOG(LOG_LEVEL) invocations will be considered for logging
     *
     * and LOG(LOG_LEVEL, USER_COMPONENT_ID) calls are considered too for registered components
     * (i.e. for component IDs specified in the vector of components).
     */
    bool initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel, const std::list<ComponentAttributes>& components);

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
     * log check - non-component (single-arg) and component (double-arg) specific
     */
    bool isLoggable(const Log::LOG_LEVEL& level);
    bool isLoggable(const Log::LOG_LEVEL& level, const uint32_t& componentId);

    /**
     * Get/Set the log threshold for all LOG messages without a component specified, i.e. calls such as
     * LOG(Log::INF) << "general log message"
     */
    void setNonComponentLogLevel(const Log::LOG_LEVEL& level);
    Log::LOG_LEVEL getNonComponentLogLevel(void);

    /**
     * Get/Set the log threshold for all LOG messages where a component is specified, i.e. calls such as
     * LOG(Log::INF, USER_DEFINED_COMPONENT_ID_1) << "log message tagged with component id"
     *
     * Note getComponentLogLevel returns false if componentId not registered.
     */
    const std::list<ComponentAttributes> getComponentLogsList();
    bool setComponentLogLevel(const uint32_t& componentId, const LOG_LEVEL& level);
    bool getComponentLogLevel(const uint32_t& componentId, LOG_LEVEL& level);



    /**
     * Sets the log threshold for *all* LOG invocations...
     *
     * applies to non component-specific logging, i.e. LOG invocations like
     * LOG(Log::INF) << "general log message"
     *
     * and applies to *all* component-specific logging, i.e. LOG invocations like
     * LOG(Log::INF, USER_DEFINED_COMPONENT_ID_1) << "log message tagged with component id"
     */
    void setGlobalLogLevel(const LOG_LEVEL& level);

    /**
     * Returns the user defined string (specified via ComponentAttributes in the initializeLogging call)
     * for a given componentId.
     * If there is no component registered for that id, returns "UNKNOWN"
     */
    std::string componentIdToString(const uint32_t& componentId);
}

#endif /* LOGIT_H_ */
