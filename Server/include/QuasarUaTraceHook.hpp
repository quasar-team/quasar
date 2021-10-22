/* © Copyright CERN, Universidad de Oviedo, 2015.  All rights not expressly granted are reserved.
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

#ifdef BACKEND_UATOOLKIT

#include <srvtrace.h>

class QuasarUaTraceHook : public UaTraceHook
{
    void traceOutput(UaTrace::TraceLevel traceLevel, const char * sContent, int nModule) override;
};

#endif // BACKEND_UATOOLKIT