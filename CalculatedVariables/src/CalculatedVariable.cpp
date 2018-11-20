/*
 * CalculatedVariable.cpp
 *
 *  Created on: 19 Nov 2018
 *      Author: pnikiel
 */

#include <CalculatedVariable.h>
#include <LogIt.h>


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
        //TODO uncomment when added the engine
        //m_parser.SetVarFactory(&Engine::onParserVariableRequest, /*user data*/ this);
        m_parser.Eval(); // this compiles the expression and does 1st evaluation
    }
    catch (const mu::Parser::exception_type &e)
    {
        LOG(Log::ERR) << e.GetMsg();
        throw std::runtime_error("Calculated item instantiation failed. Problem has been logged.");
    }

    update();

}

void CalculatedVariable::update()
{
    double updatedValue = m_parser.Eval();
    UaDataValue dataValue(UaVariant(updatedValue), OpcUa_Good, UaDateTime::now(), UaDateTime::now());
    this->setValue(nullptr, dataValue, OpcUa_False);
}

}
