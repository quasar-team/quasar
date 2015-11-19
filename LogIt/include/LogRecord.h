/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogRecord.h
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

#ifndef LOGRECORD_H_
#define LOGRECORD_H_

#include <stdint.h>
#include <sstream>
#include "LogLevels.h"

class LogRecord
{
public:
    LogRecord(const std::string& file, const int& line, const Log::LOG_LEVEL& level);
    LogRecord(const std::string& file, const int& line, const Log::LOG_LEVEL& level, const uint32_t& componentId);
    virtual ~LogRecord();

    std::ostream& getStream();

private:
    static const std::string stripDirectory(const std::string& file);
    std::ostringstream& initializeStream(const std::string& file, const int& line, const Log::LOG_LEVEL& level);
    std::ostringstream m_stream;
};



#endif /* LOGRECORD_H_ */
