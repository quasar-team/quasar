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


// internal - not exposed via API.
// void registerComponents(const std::list<ComponentAttributes>& components)
// {
// 	if(!LogItInstance::instanceExists()) return;
//     for(std::list<ComponentAttributes>::const_iterator it = components.begin(); it != components.end(); ++it)
//     {
//         ComponentAttributes component = *it;
//         LogItInstance::getInstance()->m_sComponents.insert( std::map<uint32_t, ComponentAttributes>::value_type(component.getId(), component) );
//     }
// }


bool Log::initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel)
{
	if(LogItInstance::instanceExists()) return false;
	LogItInstance::createInstance();
    initializeSinks();
    setNonComponentLogLevel(nonComponentLogLevel);
    return true;
}

// bool Log::initializeLogging(const Log::LOG_LEVEL& nonComponentLogLevel, const std::list<ComponentAttributes>& components)
// {
// 	if(!initializeLogging(nonComponentLogLevel)) return false;
//     registerComponents(components);
//     return true;;
// }

bool Log::initializeDllLogging(LogItInstance* remoteInstance)
{
	return LogItInstance::setInstance(remoteInstance);
}

bool Log::isLoggable(const Log::LOG_LEVEL& level)
{
    if(!LogItInstance::instanceExists()) return false;
    return level >= LogItInstance::getInstance()->m_nonComponentLogLevel;
}

//bool Log::isLoggable(const Log::LOG_LEVEL& level, const std::string& componentId)
bool Log::isLoggable(const Log::LOG_LEVEL& level, const std::size_t& componentId)
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

// const std::unordered_map<std::string, Log::LOG_LEVEL> getComponentLogsList()
const std::unordered_map<std::size_t, Log::LOG_LEVEL> getComponentLogsList()
{
  return LogItInstance::getInstance()->m_componentLevels;
}

bool Log::addComponent(const char* componentName, const LOG_LEVEL& level) {
  if(!LogItInstance::instanceExists()) return false;
  // std::size_t key = std::hash<const char*>{}(componentName);
  // std::size_t key = Log::pointer_hash<char>{}(componentName);
  std::size_t key = djb2(componentName);
  LogItInstance::getInstance()->m_componentLevels[key] = level;
  std::cout << "name=" << componentName << " key=" << key << " level= " << level << " size=" << LogItInstance::getInstance()->m_componentLevels.size() << std::endl;
  return true;
}


bool Log::setComponentLogLevel(const char* componentName, const LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return false;
  
  // std::size_t key = std::hash<const char*>{}(componentName);
  // std::size_t key = Log::pointer_hash<char>{}(componentName);
  std::size_t key = djb2(componentName);
  std::unordered_map<std::size_t,Log::LOG_LEVEL>::iterator pos = LogItInstance::getInstance()->m_componentLevels.find(key);
  if ( pos == LogItInstance::getInstance()->m_componentLevels.end()) return false;

  pos->second = level;
  return true;
}

bool Log::getComponentLogLevel(const std::size_t& componentId, LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return false;

  std::unordered_map<std::size_t,Log::LOG_LEVEL>::iterator pos = LogItInstance::getInstance()->m_componentLevels.find(componentId);
  if( pos == LogItInstance::getInstance()->m_componentLevels.end()) {
    std::cerr << "Error, LogIt: log component " << componentId << " does not exist!" << std::endl;
    return false;
  }

  level = pos->second;
  return true;
}

void Log::setGlobalLogLevel(const LOG_LEVEL& level)
{
  if(!LogItInstance::instanceExists()) return;
  
  LogItInstance::getInstance()->m_nonComponentLogLevel = level;
  for(std::unordered_map<std::size_t, Log::LOG_LEVEL>::iterator pos = LogItInstance::getInstance()->m_componentLevels.begin(); pos != LogItInstance::getInstance()->m_componentLevels.end(); ++pos)
    {
      pos->second = level;
    }
}

// std::string Log::componentIdToString(const uint32_t& componentId)
// {
// 	if(!LogItInstance::instanceExists()) return "UNKNOWN";

//     std::map<uint32_t, ComponentAttributes>::iterator pos = LogItInstance::getInstance()->m_sComponents.find(componentId);
//     if(pos == LogItInstance::getInstance()->m_sComponents.end()) return "UNKNOWN";

//     return pos->second.getName();
// }

unsigned long djb2(const char* str) // DJB2, http://www.cse.yorku.ca/~oz/hash.html
{
  // std::cout << "djb2: " << str << std::endl;
  unsigned long hash = 5381;
  int c;
  while ( c = *str++ ) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

// uint32_t fnv1a(const char* text, uint32_t hash) // http://create.stephan-brumme.com/fnv-hash/
// {
//   assert(text);
//   while (*text) hash = fnv1a((unsigned char)*text++, hash);
//   return hash;
// }
