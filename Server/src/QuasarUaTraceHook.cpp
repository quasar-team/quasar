/* © Copyright CERN, 2021.  All rights not expressly granted are reserved.
 * QuasarUaTraceHook.cpp
 *
 *  Created on: Oct 22, 2021
 * 		Author: Paris Moschovakos <paris.moschovakos@cern.ch>
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

#ifdef BACKEND_UATOOLKIT

#include <QuasarUaTraceHook.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <Instrumentor.hpp>

void QuasarUaTraceHook::traceOutput(UaTrace::TraceLevel traceLevel, const char * sContent, int nModule)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint = std::chrono::high_resolution_clock::now();
    switch (traceLevel) {
    case UaTrace::TraceLevel::NoTrace: ;
        break;
    case UaTrace::TraceLevel::Errors: logMessage("ERR", sContent);
        break;
    case UaTrace::TraceLevel::Warning: logMessage("WRN", sContent);
        break;
    case UaTrace::TraceLevel::Info: logMessage("INF", sContent);
        break;
    case UaTrace::TraceLevel::InterfaceCall:
    case UaTrace::TraceLevel::CtorDtor:
    case UaTrace::TraceLevel::ProgramFlow:
    case UaTrace::TraceLevel::Data:
    default: logMessage("TRC", sContent);
        break;
    }
    auto endTimepoint = std::chrono::high_resolution_clock::now();

    long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
    long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
    m_duration += (end - start);
    m_iteration++;

};

 void QuasarUaTraceHook::logMessage(const std::string & traceLevel, const char * sContent)
{

    std::cout << getCurrentDateAndTime() << "[" << baseName(__FILE__) << ":" << __LINE__ << ", " << traceLevel << "] " << "UaTrace: " << sContent << std::endl;

}

 std::string QuasarUaTraceHook::getCurrentDateAndTime()
{

    using namespace std::chrono;
    auto currentDateAndTime_tp = std::chrono::system_clock::now();
    std::time_t currentDateAndTime = std::chrono::system_clock::to_time_t(currentDateAndTime_tp);
    tm currentDateAndTime_tm = *localtime(&currentDateAndTime);

    std::ostringstream now;
    now << currentDateAndTime_tm.tm_year + 1900 << '-'
        << currentDateAndTime_tm.tm_mon + 1 << '-'
        << currentDateAndTime_tm.tm_mday << ' '
        << currentDateAndTime_tm.tm_hour << ':'
        << currentDateAndTime_tm.tm_min << ':'
        << currentDateAndTime_tm.tm_sec << ' ';

    return now.str();

}

 std::string QuasarUaTraceHook::baseName(const std::string & filepath)
{

    return filepath.substr(filepath.find_last_of("/\\") + 1);

}

#endif // BACKEND_UATOOLKIT