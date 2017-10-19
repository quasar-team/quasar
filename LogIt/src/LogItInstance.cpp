/*
 * LogItInstance.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: bfarnham
 */

#include "LogItInstance.h"
#include <iostream>
#include <mutex>

using std::cerr;
using std::endl;

LogItInstance* LogItInstance::g_sLogItInstance(NULL); // initially - set by any LogIt::initialize*Logging() call.

LogItInstance::LogItInstance()
:m_isLoggingInitialized(false), m_nonComponentLogLevel(Log::INF), m_sComponents(new ComponentMap())
{}

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

/**
 * The logging component map is controlled by a shared_ptr: shared_ptr ensures that
 * operations in play (e.g. log component look up etc) will complete before the object
 * is destroyed (shared_ptr guarantees only one thread - the last to release - will destroy
 * the object.
 *
 */
void LogItInstance::addLogItComponent(ComponentAttributes& newComponent)
{
	static std::mutex g_sLogItComponentAdditionLock;

	// only a single thread at a time can change the contents of the LogItComponent
	g_sLogItComponentAdditionLock.lock();
	{
		// clone existing map, add new component and reset the shared ptr to hold the new map.
		ComponentMap* newComponentMap = new ComponentMap(*m_sComponents);
		newComponentMap->emplace(newComponent.getId(), newComponent);
		m_sComponents.reset(newComponentMap);
	}
	g_sLogItComponentAdditionLock.unlock();
}

LogItInstance::ComponentMapPtr LogItInstance::getComponentMap()
{
	return m_sComponents;
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
		std::cerr << "Failed to create new LogItInstance, already have instance ["<<getInstance()<<"]. Ignoring (and returning instance)" << endl;
	}
	else
	{
		g_sLogItInstance = new LogItInstance();
	}
	return getInstance();
}


