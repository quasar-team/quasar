/*
 * LogItInstance.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: bfarnham
 */

#include "LogItInstance.h"
#include <iostream>

LogItInstance* LogItInstance::g_sLogItInstance(NULL); // initially

LogItInstance::LogItInstance()
:m_isLoggingInitialized(false), m_nonComponentLogLevel(Log::INF)
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

bool LogItInstance::setInstance(LogItInstance* remoteInstance)
{
	if(!remoteInstance) return false;
	if(instanceExists())
	{
		std::cerr << "Failed to set LogItInstance with remoteInstance ["<<remoteInstance<<"], already have incumbent instance ["<<getInstance()<<"]. Ignoring (and returning false)";
		return false;
	}
	LogItInstance::g_sLogItInstance = remoteInstance;
	return true;
}

LogItInstance* LogItInstance::createInstance()
{
	if(instanceExists())
	{
		std::cerr << "Failed to create new LogItInstance, already have instance ["<<getInstance()<<"]. Ignoring (and returning instance)";
		return false;
	}

	LogItInstance::g_sLogItInstance = new LogItInstance();
	return LogItInstance::g_sLogItInstance;
}


