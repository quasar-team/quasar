/* © Copyright CERN, 2021.  All rights not expressly granted are reserved.
 * QuasarUaTraceHook.hpp
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

#pragma once

#ifdef BACKEND_UATOOLKIT

#include <srvtrace.h>
#include <string>

class QuasarUaTraceHook : public UaTraceHook
{
public:
    void traceOutput(UaTrace::TraceLevel traceLevel, const char * sContent, int nModule) override;
private:
    inline const std::string getCurrentDateAndTime();
    inline const std::string baseName(const std::string & filepath);
    inline void logMessage(const std::string & traceLevel, int nModule, const char * sContent);
    enum ModuleNumber
    {
        Shredder = 0,
        UaStack = 5
    };
};

#endif // BACKEND_UATOOLKIT