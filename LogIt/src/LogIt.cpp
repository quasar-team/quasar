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
#include <boost/algorithm/string/case_conv.hpp>

#include "ComponentAttributes.h"
#include "LogSinks.h"
#include "LogLevels.h"
#include "LogItInstance.h"

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

bool Log::isLoggable(const Log::LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return false;
  return level >= LogItInstance::getInstance()->m_nonComponentLogLevel;
}

bool Log::isLoggable(const Log::LOG_LEVEL& level, const unsigned long& componentId)
{
  if(!LogItInstance::instanceExists()) return false;
  LOG_LEVEL componentLogLevel;
  if(!getComponentLogLevel(componentId, componentLogLevel)) return false; // unregistered component id.
  
  return level >= componentLogLevel;
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

const std::unordered_map<unsigned long, Log::LOG_LEVEL> getComponentLogsList()
{
  return *LogItInstance::getInstance()->m_componentLevels;
}

bool Log::addComponent(const char* componentName, const LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return false;
  // unsigned long key = std::hash<const char*>{}(componentName);
  // unsigned long key = Log::pointer_hash<char>{}(componentName);
  unsigned long key = djb2(componentName);

  logLevelMap *mapCopy = new logLevelMap(*LogItInstance::getInstance()->m_componentLevels);
  (*mapCopy)[key] = level;
  logLevelMap *oldMapPointer = LogItInstance::getInstance()->m_componentLevels.exchange(mapCopy); // thread-safe replacement of map

  // std::cout << "name=" << componentName << " key=" << key << " level= " << level
  // 	    << " size=" << mapCopy->size() << " was size=" << oldMapPointer->size() << std::endl;
  if (mapCopy->size() == oldMapPointer->size()) {
    std::cout << "Log::addComponent: Warning, component with name=" << componentName
	      << " seems to exist already (hash=" << key << "). This should not happen." << std::endl;
  }
  delete oldMapPointer;

  return true;
}


bool Log::setComponentLogLevel(const char* componentName, const LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return false;
  
  // unsigned long key = std::hash<const char*>{}(componentName);
  // unsigned long key = Log::pointer_hash<char>{}(componentName);
  unsigned long key = djb2(componentName);

  logLevelMap *levels = LogItInstance::getInstance()->m_componentLevels;
  std::unordered_map<unsigned long,Log::LOG_LEVEL>::iterator pos = levels->find(key);
  if ( pos == levels->end()) return false;

  pos->second = level;
  return true;
}

bool Log::getComponentLogLevel(const unsigned long& componentId, LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return false;

  logLevelMap *levels = LogItInstance::getInstance()->m_componentLevels;
  std::unordered_map<unsigned long,Log::LOG_LEVEL>::iterator pos = levels->find(componentId);
  if ( pos == levels->end()) {
    std::cerr << "Error, LogIt: log component " << componentId << " does not exist!" << std::endl;
    return false;
  }

  level = pos->second;
  return true;
}

bool Log::getComponentLogLevel(const char* componentName, LOG_LEVEL& level)
{
  return getComponentLogLevel(djb2(componentName), level);
}


void Log::setGlobalLogLevel(const LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return;
  
  logLevelMap *levels = LogItInstance::getInstance()->m_componentLevels;
  LogItInstance::getInstance()->m_nonComponentLogLevel = level;
  for(std::unordered_map<unsigned long, Log::LOG_LEVEL>::iterator pos = levels->begin(); pos != levels->end(); ++pos)
    {
      pos->second = level;
    }
}

//
// DJB2 hash function by Dan J. Bernstein, taken from http://www.cse.yorku.ca/~oz/hash.html
//
unsigned long djb2(const char* str) 
{
  unsigned long hash = 5381;
  int c;
  while ( (c = *str++) ) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}
