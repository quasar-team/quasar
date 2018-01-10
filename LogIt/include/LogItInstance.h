/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogItInstance.h
 *
 *  Created on: Apr 4, 2016
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
#ifndef THEDYNLIB_LOGIT_LOGITINSTANCE_H_
#define THEDYNLIB_LOGIT_LOGITINSTANCE_H_

#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "LogItStaticDefinitions.h"
#include "LogLevels.h"
#include "ComponentAttributes.h"
#include "LogSinks.h"

class LogItInstance
{
private:
	typedef std::vector<ComponentAttributes> LoggingComponentsArray;
	typedef std::unordered_map<std::string, Log::LogComponentHandle> MapOfComponentNameToHandle;
	typedef std::shared_ptr<MapOfComponentNameToHandle> MapOfComponentNameToHandlePtr;

public:
	static bool instanceExists();
	static LogItInstance* getInstance();
	static bool setInstance(LogItInstance* remoteInstance);
	static LogItInstance* createInstance();

private:
	LogItInstance();
	virtual ~LogItInstance();
	static LogItInstance* g_sLogItInstance;

public:
	ComponentAttributes& registerLoggingComponent(const std::string& componentName, const Log::LOG_LEVEL& nonComponentLogLevel);

	bool getComponentAttributes(const Log::LogComponentHandle& componentHandle, ComponentAttributes*& componentAttributes);
	bool getComponentHandle(const std::string& componentName, Log::LogComponentHandle& componentHandle);
	std::map<Log::LogComponentHandle, std::string> getLoggingComponentsList() const;

	bool m_isLoggingInitialized;
	LogSinks m_logSinksInstance;
	Log::LOG_LEVEL m_nonComponentLogLevel;

private:
	LoggingComponentsArray m_components;
	MapOfComponentNameToHandlePtr m_componentNames;
	mutable std::mutex m_componentsLock;
};

#endif /* THEDYNLIB_LOGIT_LOGITINSTANCE_H_ */
