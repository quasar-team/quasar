/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * CalculatedVariable.cpp
 *
 *  Created on: 19 Nov 2018
 *      Author: pnikiel, Paris Moschovakos
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

#include <math.h>

#include <algorithm>

#include <CalculatedVariable.h>
#include <LogIt.h>
#include <CalculatedVariablesEngine.h>
#include <CalculatedVariablesLogComponentId.h>
#include <ParserVariableRequestUserData.h>

namespace CalculatedVariables
{

CalculatedVariable::CalculatedVariable(
    const UaNodeId&    nodeId,
    const UaString&    name,
    OpcUa_UInt16       browseNameNameSpaceIndex,
    NodeManagerConfig* pNodeConfig,
    const std::string& formula,
    bool               isBoolean,
    bool               hasStatusFormula,
    const std::string& statusFormula,
    UaMutexRefCounted* pSharedMutex,
    bool               autoUpdateEnabled):
            AddressSpace::ChangeNotifyingVariable(
                    nodeId,
                    name,
                    browseNameNameSpaceIndex,
                    UaVariant(),
                    OpcUa_AccessLevels_CurrentRead,
                    pNodeConfig,
                    pSharedMutex),
                    m_isBoolean(isBoolean),
                    m_hasStatusFormula(hasStatusFormula),
                    m_notifiedVariable(nullptr),
                    m_autoUpdateEnabled(autoUpdateEnabled)
{
    this->initializeParser(m_valueParser, formula, ParserVariableRequestUserData::Type::Value);
    if (m_hasStatusFormula)
        this->initializeParser(m_statusParser, statusFormula, ParserVariableRequestUserData::Type::Status);

    UaDataValue dataValue(UaVariant(), OpcUa_BadWaitingForInitialData, UaDateTime::now(), UaDateTime::now());
    this->setValue(nullptr, dataValue, OpcUa_False);

}

// Performs an update of the calculated variable, regardless of whether automatic
// updates are enabled. This allows the user to manually trigger a recalculation when
// automatic updates are disabled.
void CalculatedVariable::triggerRecalculation()
{
    LOG(Log::TRC, logComponentId) << "triggerRecalculation() on " << this->nodeId().toString().toUtf8();
    calculate();
}

// this is called whenever value OR status variable bound to this variable has changed
// validation rule: if any of value inputs is not good then propagate it and do not do new computation (publish NULL)
// otherwise do the computation and publish status according to status formula (if some of status formula are wrong
// then publish uncertain, otherwise publish good
void CalculatedVariable::update()
{
    LOG(Log::TRC, logComponentId) << "update() on " << this->nodeId().toString().toUtf8();

    if (!m_autoUpdateEnabled) return;
    calculate();
}

void CalculatedVariable::calculate()
{
    LOG(Log::TRC, logComponentId) << "calculate() on " << this->nodeId().toString().toUtf8();

    for (const ParserVariable* variable : m_valueVariables)
    {
        if (variable->state() != ParserVariable::State::Good)
        {
            OpcUa_StatusCode statusCode;
            if (variable->state() == ParserVariable::State::WaitingInitialData)
                statusCode = OpcUa_BadWaitingForInitialData;
            else if (variable->state() == ParserVariable::State::Bad)
                statusCode = OpcUa_Bad;
            else
            {
                LOG(Log::ERR, logComponentId) << "Enum value not handled! Implement me! Or contact Piotr.";
                statusCode = OpcUa_Bad;
            }
            UaDataValue dataValue(UaVariant(), statusCode, UaDateTime::now(), UaDateTime::now());
            this->setValue(/*session*/nullptr, dataValue, OpcUa_False);
            return;
        }
    }
    // so looks like every dependent value was good... can perform the value calculation.
    // now check what status formula (if used) evaluates to
    UaStatus finalStatus = OpcUa_Good;
    if (m_hasStatusFormula)
    {
        double status = m_statusParser.Eval();
        LOG(Log::TRC, logComponentId) << "status evaluates to: " << status;
        finalStatus = (status != 0) ? OpcUa_Good : OpcUa_Bad; // conversion of double to OPC-UA status code
    }

    double updatedValue = m_valueParser.Eval();
    UaVariant variant;
    if (m_isBoolean)
        variant.setBool(updatedValue != 0);
    else
        variant.setDouble(updatedValue);
    UaDataValue dataValue (variant, finalStatus.statusCode(), UaDateTime::now(), UaDateTime::now());
    LOG(Log::TRC, logComponentId) << finalStatus.toString().toUtf8();
    this->setValue(/*session*/nullptr, dataValue, OpcUa_False);
}

//! Initializes parser, handles potential muParser-relevant exceptions throwing std except in exchange
void CalculatedVariable::initializeParser(
        mu::Parser& parser,
        const std::string& formula,
        ParserVariableRequestUserData::Type formulaType)
{
    const std::string typeAsStr = formulaType == ParserVariableRequestUserData::Type::Value ? "value" : "status";
    try
    {   
        parser.DefineFun("pow", [](double x, double y){return pow(x, y);}); // power to be available also as pow(x,y) in addition to x^y
        parser.DefineNameChars("0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.");
        parser.SetExpr(formula);
        // after SetExpr parser knows the symbols(tokens),
        // we can supply constants.
        auto usedVariables = parser.GetUsedVar();
        LOG(Log::TRC, logComponentId) << "At CalculatedVariable " << this->nodeId().toString().toUtf8() << " recognized these operands:";
        for (auto& x : usedVariables)
        {
            // check if we're dealing with a constant?
            LOG(Log::TRC, logComponentId) << "name: " << x.first; // no sense to print the value as it is not initialized yet
            if (Engine::isConstantDefined(x.first))
            {
                double value = Engine::getValueOfConstant(x.first);
                LOG(Log::TRC, logComponentId) << "Recognized use of constant, name: " << x.first << " value: " << value;
                parser.DefineConst(x.first, value);
            }
        }
        ParserVariableRequestUserData userData;
        userData.type = formulaType;
        userData.requestor = this;
        parser.SetVarFactory(&Engine::parserVariableRequestHandler, &userData );
        parser.Eval(); // this compiles the expression and does 1st evaluation
    }
    catch(const mu::Parser::exception_type &e)
    {
        LOG(Log::ERR, logComponentId) << "At CalculatedVariable " <<
                this->nodeId().toString().toUtf8() << " in " << typeAsStr << " formula : "
                << e.GetExpr() << ": " << e.GetMsg();
        throw std::runtime_error("Calculated item instantiation failed. Problem has been logged.");
    }
}

void CalculatedVariable::addDependentVariableForValue(ParserVariable* variable)
{
    /* Adding same variable twice wouldn't buy us anything */
    if (std::find(m_valueVariables.begin(), m_valueVariables.end(), variable) == m_valueVariables.end())
        m_valueVariables.push_back(variable);
}

void CalculatedVariable::addDependentVariableForStatus(ParserVariable* variable)
{
    /* Adding same variable twice wouldn't buy us anything */
    if (std::find(m_statusVariables.begin(), m_statusVariables.end(), variable) == m_statusVariables.end())
        m_statusVariables.push_back(variable);
}

}
