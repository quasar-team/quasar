/* © Copyright CERN, 2015.  All rights not expressly granted are reserved.
 * LogSinks.h
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

#ifndef SRC_INCLUDE_LOGSINKS_H_
#define SRC_INCLUDE_LOGSINKS_H_

#include <string>
#include <vector>
#include "LogSinkInterface.h"

class LogSinks
{
public:
	LogSinks();
	virtual ~LogSinks();
    void addSink(LogSinkInterface* sink);
    void logMessage(const std::string& logMsg);

private:
    std::vector<LogSinkInterface*> m_sinks;
};

#endif /* SRC_INCLUDE_LOGSINKS_H_ */
