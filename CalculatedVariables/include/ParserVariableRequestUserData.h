/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * ParserVariableRequestUserData.h
 *
 *  Created on: 22 Nov 2018
 *      Author: pnikiel
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

#ifndef CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLEREQUESTUSERDATA_H_
#define CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLEREQUESTUSERDATA_H_

namespace CalculatedVariables
{

class CalculatedVariable;

struct ParserVariableRequestUserData
{
    enum Type
    {
        Value,
        Status
    };
    Type type;
    CalculatedVariable* requestor;
};

}



#endif /* CALCULATEDVARIABLES_INCLUDE_PARSERVARIABLEREQUESTUSERDATA_H_ */
