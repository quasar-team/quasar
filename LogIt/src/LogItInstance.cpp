/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogItInstance.cpp
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
#include "LogItInstance.h"
#include "LogItStaticDefinitions.h"
#include <iostream>
#include <sstream>

using std::cerr;
using std::endl;
using std::map;
using std::string;
using std::ostringstream;

LogItInstance* LogItInstance::g_sLogItInstance(NULL); // initially - set by any LogIt::initialize*Logging() call.

LogItInstance::LogItInstance()
:m_isLoggingInitialized(false), m_nonComponentLogLevel(Log::INF), m_componentNames(new MapOfComponentNameToHandle)
{
	m_components.reserve(Log::g_sMaxComponentIdCount);
}

LogItInstance::~LogItInstance()
{}

bool LogItInstance::instanceExists()
{
	return LogItInstance::g_sLogItInstance != NULL;
}

LogItInstance* LogItInstance::getInstance()
{
	return LogItInstance::g_sLogItInstance;
}

bool LogItInstance::setInstance(LogItInstance* remoteInstance)
{
	if(!remoteInstance)
	{
		cerr << "Failed to set LogItInstance with NULL remoteInstance. Ignoring (and returning false)" << endl;
		return false;
	}
	if(instanceExists())
	{
		if(getInstance() == remoteInstance) return true; // fine - do nothing.
		cerr << "Failed to set LogItInstance with remoteInstance ["<<remoteInstance<<"], already have incumbent instance ["<<getInstance()<<"]. Ignoring (and returning false)" << endl;
		return false;
	}
	g_sLogItInstance = remoteInstance;
	return true;
}

LogItInstance* LogItInstance::createInstance()
{
	if(instanceExists())
	{
		cerr << "Failed to create new LogItInstance, already have instance ["<<getInstance()<<"]. Ignoring (and returning instance)" << endl;
	}
	else
	{
		g_sLogItInstance = new LogItInstance();
	}
	return getInstance();
}

ComponentAttributes& LogItInstance::registerLoggingComponent(const string& componentName, const Log::LOG_LEVEL& nonComponentLogLevel)
{
	// serialization: mutex ownership required to change contents of the vector of logging components
	std::lock_guard<decltype(m_componentsLock)> scopedLock(m_componentsLock);

	// check whether component is already registered (if so, should be a handle, handle ids the component attributes
	Log::LogComponentHandle existingComponentHandle = Log::INVALID_HANDLE;
	if(getComponentHandle(componentName, existingComponentHandle))
	{
		ComponentAttributes* componentAttributes = nullptr;
		if(getComponentAttributes(existingComponentHandle, componentAttributes))
		{
			return *componentAttributes;
		}
		else
		{
			ostringstream msg;
			msg << __FUNCTION__<<" FAILED: logging component ["<<componentName<<"] has handle ["<<existingComponentHandle<<"], but no attributes found. Probably a programming error. Current entry count ["<<m_components.size()<<"]";
			throw std::runtime_error(msg.str());
		}
	}

	// is there room to add this component?
	if(m_components.size() >= Log::g_sMaxComponentIdCount)
	{
		ostringstream msg;
		msg << __FUNCTION__<<" FAILED: cannot add logging component ["<<componentName<<"] to components list, the list is full (entry count ["<<m_components.size()<<"])";
		throw std::runtime_error(msg.str());
	}

	// Note: a components LogComponentHandle is its index in the vector of logging components.
	const Log::LogComponentHandle newComponentHandle(m_components.size());
	m_components.emplace_back(ComponentAttributes(newComponentHandle, componentName, nonComponentLogLevel));

	// clone existing map, add new component and reset shared pointer to use new map.
	MapOfComponentNameToHandle* newComponentNames = new MapOfComponentNameToHandle(*m_componentNames);
	newComponentNames->emplace(componentName, newComponentHandle);
	m_componentNames.reset(newComponentNames);

	return m_components[newComponentHandle];
}

bool LogItInstance::getComponentAttributes(const Log::LogComponentHandle& componentHandle, ComponentAttributes*& componentAttributes)
{
	if(componentHandle >= m_components.size()) return false;

	componentAttributes = &(m_components[componentHandle]);
	return true;
}

bool LogItInstance::getComponentHandle(const std::string& componentName, Log::LogComponentHandle& componentHandle)
{
	const auto it = m_componentNames->find(componentName);
	if(it == m_componentNames->end()) return false;

	componentHandle = it->second;
	return true;
}

map<Log::LogComponentHandle, string> LogItInstance::getLoggingComponentsList() const
{
	// serialization: mutex ownership required to loop through vector of logging components
	std::lock_guard<decltype(m_componentsLock)> scopedLock(m_componentsLock);

	map<Log::LogComponentHandle, string> result;

	for(size_t i=0; i<m_components.size(); ++i)
	{
		const ComponentAttributes& componentAttributes = m_components[i];
		result[componentAttributes.getHandle()] = componentAttributes.getName();
	}

	return result;
}
