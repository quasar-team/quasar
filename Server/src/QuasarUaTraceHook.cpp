/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
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

#include <LogIt.h>
#include <QuasarUaTraceHook.hpp>

void QuasarUaTraceHook::traceOutput(UaTrace::TraceLevel traceLevel, const char * sContent, int nModule)
{
    switch (traceLevel) {
    case UaTrace::TraceLevel::NoTrace: ;
        break;
    case UaTrace::TraceLevel::Errors: LOG(Log::ERR) << "UaTrace: " << sContent;
        break;
    case UaTrace::TraceLevel::Warning: LOG(Log::WRN) << "UaTrace: " << sContent;
        break;
    case UaTrace::TraceLevel::Info: LOG(Log::INF) << "UaTrace: " << sContent;
        break;
    case UaTrace::TraceLevel::InterfaceCall: 
    case UaTrace::TraceLevel::CtorDtor: 
    case UaTrace::TraceLevel::ProgramFlow: 
    case UaTrace::TraceLevel::Data:
    default: LOG(Log::TRC) << "UaTrace: " << sContent;
        break;
    }
};

#endif // BACKEND_UATOOLKIT