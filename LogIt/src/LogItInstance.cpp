/*
 * LogItInstance.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: bfarnham
 */

#include "LogItInstance.h"
#include <iostream>

using std::cerr;
using std::endl;

LogItInstance* LogItInstance::g_sLogItInstance(NULL); // initially - set by any LogIt::initialize*Logging() call.

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


