/*
 * LogItInstance.h
 *
 *  Created on: Apr 4, 2016
 *      Author: bfarnham
 */

#ifndef THEDYNLIB_LOGIT_LOGITINSTANCE_H_
#define THEDYNLIB_LOGIT_LOGITINSTANCE_H_

#include <unordered_map>
#include "LogLevels.h"
#include "ComponentAttributes.h"
#include "LogSinks.h"

class LogItInstance
{
public:
	typedef std::unordered_map<uint64_t, ComponentAttributes> ComponentMap;

	static bool instanceExists();
	static LogItInstance* getInstance();
	static bool setInstance(LogItInstance* remoteInstance);
	static LogItInstance* createInstance();

	bool m_isLoggingInitialized;
	LogSinks m_logSinksInstance;
	ComponentMap m_sComponents;
	Log::LOG_LEVEL m_nonComponentLogLevel;

private:
	LogItInstance();
	virtual ~LogItInstance();

	static LogItInstance* g_sLogItInstance;
};

#endif /* THEDYNLIB_LOGIT_LOGITINSTANCE_H_ */
