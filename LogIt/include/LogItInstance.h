/*
 * LogItInstance.h
 *
 *  Created on: Apr 4, 2016
 *      Author: bfarnham
 */

#ifndef THEDYNLIB_LOGIT_LOGITINSTANCE_H_
#define THEDYNLIB_LOGIT_LOGITINSTANCE_H_

#include <atomic>
#include <unordered_map>
#include "LogLevels.h"
#include "LogSinks.h"

typedef std::unordered_map<unsigned long, Log::LOG_LEVEL> logLevelMap;

class LogItInstance
{
public:
	static bool instanceExists();
	static LogItInstance* getInstance();
	static bool setInstance(LogItInstance* remoteInstance);
	static LogItInstance* createInstance();

	bool m_isLoggingInitialized;
	LogSinks m_logSinksInstance;
	std::atomic<logLevelMap*> m_componentLevels;
	Log::LOG_LEVEL m_nonComponentLogLevel;

private:
	LogItInstance();
	virtual ~LogItInstance();

	static LogItInstance* g_sLogItInstance;
};

#endif /* THEDYNLIB_LOGIT_LOGITINSTANCE_H_ */
