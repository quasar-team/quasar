/*
 * CalculatedVariable.cpp
 *
 *  Created on: 19 Nov 2018
 *      Author: pnikiel
 */

#include <algorithm>

#include <CalculatedVariable.h>
#include <LogIt.h>
#include <CalculatedVariablesEngine.h>
#include <CalculatedVariablesLogComponentId.h>


namespace CalculatedVariables
{

CalculatedVariable::CalculatedVariable(
    const UaNodeId&    nodeId,
    const UaString&    name,
    OpcUa_UInt16       browseNameNameSpaceIndex,
    NodeManagerConfig* pNodeConfig,
    const std::string& formula,
    UaMutexRefCounted* pSharedMutex):
            AddressSpace::ChangeNotifyingVariable(
                    nodeId,
                    name,
                    browseNameNameSpaceIndex,
                    UaVariant(),
                    OpcUa_AccessLevels_CurrentRead,
                    pNodeConfig,
                    pSharedMutex)
{
    try
    {
        m_parser.DefineNameChars("0123456789_"\
                               "abcdefghijklmnopqrstuvwxyz"\
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ.");
        m_parser.SetExpr(formula);
        m_parser.SetVarFactory(&Engine::parserVariableRequestHandler, /*user data*/ this);
        m_parser.Eval(); // this compiles the expression and does 1st evaluation
    }
    catch (const mu::Parser::exception_type &e)
    {
        LOG(Log::ERR, logComponentId) << "At CalculatedVariable " <<
                this->nodeId().toString().toUtf8() << ": "
                << e.GetExpr() << ": " << e.GetMsg();
        throw std::runtime_error("Calculated item instantiation failed. Problem has been logged.");
    }

    UaDataValue dataValue(UaVariant(), OpcUa_BadWaitingForInitialData, UaDateTime::now(), UaDateTime::now());
    this->setValue(nullptr, dataValue, OpcUa_False);

}

void CalculatedVariable::update()
{
    LOG(Log::TRC, logComponentId) << "update() on " << this->nodeId().toString().toUtf8();
    for (const ParserVariable* variable : m_parserVariables)
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
    // so looks like every dependent value was good...

    double updatedValue = m_parser.Eval();
    UaDataValue dataValue(UaVariant(updatedValue), OpcUa_Good, UaDateTime::now(), UaDateTime::now());
    this->setValue(/*session*/nullptr, dataValue, OpcUa_False);

}

}
