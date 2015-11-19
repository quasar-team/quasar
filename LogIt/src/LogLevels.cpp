/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogLevels.cpp
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

#include "LogLevels.h"
#include <boost/algorithm/string.hpp>
#include <LogIt.h>

std::string Log::logLevelToString(const LOG_LEVEL& level)
{
    switch(level)
    {
        case(TRC): return "TRC";
        case(DBG): return "DBG";
        case(INF): return "INF";
        case(WRN): return "WRN";
        case(ERR): return "ERR";
        default  : return "UNKNOWN";
    }
}

bool Log::logLevelFromString(const std::string &s, LOG_LEVEL &out)
{
	const std::string levelString = boost::to_upper_copy(s);

	if (levelString=="ERR" || levelString=="ERROR" )	out=Log::ERR;
	else if	(levelString=="WRN" || levelString=="WARN" || levelString=="WARNING" ) out=Log::WRN;
	else if	(levelString=="INF" || levelString=="INFO" || levelString=="INFORMATION") out = Log::INF;
	else if	(levelString=="DBG" || levelString=="DEBUG")	out = Log::DBG;
	else if	(levelString=="TRC" || levelString=="TRACE") out = Log::TRC;
    else
    {
    	LOG(Log::WRN) << "failed to parse log level string ["<<s<<"] to log level";
    	return false;
    }
    return true;
}
