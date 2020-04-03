#!/usr/bin/env python
# encoding: utf-8
'''
Oracle.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

@license:
Copyright (c) 2015, CERN.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS  OR IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED
WARRANTIES  OF  MERCHANTABILITY  AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL DAMAGES
(INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR  SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON
ANY  THEORY  OF  LIABILITY,   WHETHER IN  CONTRACT, STRICT  LIABILITY,  OR  TORT
(INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT OF  THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@contact:    quasar-developers@cern.ch
'''

from transform_filters import cap_first

class Oracle():
    AllQuasarDataTypes = [
        'OpcUa_Boolean',
        'OpcUa_Byte',
        'OpcUa_SByte',
        'OpcUa_UInt16',
        'OpcUa_Int16',
        'OpcUa_UInt32',
        'OpcUa_Int32',
        'OpcUa_UInt64',
        'OpcUa_Int64',
        'OpcUa_Double',
        'OpcUa_Float',
        'UaString',
        'UaVariant',
        'UaByteString']

    NumericDataTypes = [
        'OpcUa_Byte',
        'OpcUa_SByte',
        'OpcUa_UInt16',
        'OpcUa_Int16',
        'OpcUa_UInt32',
        'OpcUa_Int32',
        'OpcUa_UInt64',
        'OpcUa_Int64',
        'OpcUa_Double',
        'OpcUa_Float']

    PassByValueDataTypes = [ # all scalar types that should be passed by value
        'OpcUa_Boolean',
        'OpcUa_Byte',
        'OpcUa_SByte',
        'OpcUa_UInt16',
        'OpcUa_Int16',
        'OpcUa_UInt32',
        'OpcUa_Int32',
        'OpcUa_UInt64',
        'OpcUa_Int64',
        'OpcUa_Double',
        'OpcUa_Float']

    # all types for which a configentry or config-made cache-variable is legit
    InitializeFromConfigurationDataTypes = [
        'OpcUa_Boolean',
        'OpcUa_Byte',
        'OpcUa_SByte',
        'OpcUa_UInt16',
        'OpcUa_Int16',
        'OpcUa_UInt32',
        'OpcUa_Int32',
        'OpcUa_UInt64',
        'OpcUa_Int64',
        'OpcUa_Double',
        'OpcUa_Float',
        'UaString']

    InitializeFromValueAndStatusDataTypes = [
        'OpcUa_Boolean',
        'OpcUa_Byte',
        'OpcUa_SByte',
        'OpcUa_UInt16',
        'OpcUa_Int16',
        'OpcUa_UInt32',
        'OpcUa_Int32',
        'OpcUa_UInt64',
        'OpcUa_Int64',
        'OpcUa_Double',
        'OpcUa_Float',
        'UaString']

    DataTypeToVariantSetter = {
        'OpcUa_Double' : 'setDouble',
        'OpcUa_Float'  : 'setFloat',
        'OpcUa_Byte'   : 'setByte',
        'OpcUa_SByte'  : 'setSByte',
        'OpcUa_Int16'  : 'setInt16',
        'OpcUa_UInt16' : 'setUInt16',
        'OpcUa_Int32'  : 'setInt32',
        'OpcUa_UInt32' : 'setUInt32',
        'OpcUa_Int64'  : 'setInt64',
        'OpcUa_UInt64' : 'setUInt64',
        'OpcUa_Boolean': 'setBool',
        'UaString'     : 'setString'
    }

    DataTypeToVariantConverter = {
        'OpcUa_Double'  : 'toDouble',
        'OpcUa_Float'   : 'toFloat',
        'OpcUa_Byte'    : 'toByte',
        'OpcUa_SByte'   : 'toSByte',
        'OpcUa_Int16'   : 'toInt16',
        'OpcUa_UInt16'  : 'toUInt16',
        'OpcUa_Int32'   : 'toInt32',
        'OpcUa_UInt32'  : 'toUInt32',
        'OpcUa_Int64'   : 'toInt64',
        'OpcUa_UInt64'  : 'toUInt64',
        'OpcUa_Boolean' : 'toBool',
        'UaByteString'  : 'toByteString'
    }

    DataTypeToBuiltinType = {
        'OpcUa_Double'   : 'OpcUaType_Double',
        'OpcUa_Float'    : 'OpcUaType_Float',
        'OpcUa_Byte'     : 'OpcUaType_Byte',
        'OpcUa_SByte'    : 'OpcUaType_SByte',
        'OpcUa_Int16'    : 'OpcUaType_Int16',
        'OpcUa_UInt16'   : 'OpcUaType_UInt16',
        'OpcUa_Int32'    : 'OpcUaType_Int32',
        'OpcUa_UInt32'   : 'OpcUaType_UInt32',
        'OpcUa_Int64'    : 'OpcUaType_Int64',
        'OpcUa_UInt64'   : 'OpcUaType_UInt64',
        'OpcUa_Boolean'  : 'OpcUaType_Boolean',
        'UaString'       : 'OpcUaType_String',
        'UaByteString'   : 'OpcUaType_ByteString',
        'UaVariant'      : 'OpcUaType_Variant'
    }


    def getDeviceLogicTypeFromQuasarType(self, quasar_data_type):
        """TODO @pnikiel We will clean-up type derivation in subsequent efforts """
        if quasar_data_type == "UaString":
            return "std::string"
        else:
            return quasar_data_type

    def getCacheVariableSetter(self, name, quasar_data_type, for_header):
        """This function is based on its XSLT version, CommonFunctions,
           fnc:varSetter from quasar 1.3.12
           TODO @pnikiel should be reworked to profit from other functions """
        source_time_stamp = 'const UaDateTime& srcTime'
        if for_header:
            source_time_stamp += '= UaDateTime::now()'
        if quasar_data_type in Oracle.PassByValueDataTypes:
            # TODO @pnikiel BELOW: when settled down, we should remove
            # const from PassByValue things as that looks stupid
            return 'set{0}( const {1} value, OpcUa_StatusCode statusCode, {2} )'.format(cap_first(name), quasar_data_type, source_time_stamp)
        elif quasar_data_type is None:  # formerly null
            return 'setNull{0}( OpcUa_StatusCode statusCode, {1})'.format(
                cap_first(name),
                source_time_stamp)
        else:
            return 'set{0}( const {1}& value, OpcUa_StatusCode statusCode, {2})'.format(cap_first(name), quasar_data_type, source_time_stamp)

    def getCacheVariableSetterArray(self, name, quasar_data_type, for_header):
        """ TODO: description """
        source_time_stamp = 'const UaDateTime& srcTime'
        if for_header:
            source_time_stamp += '= UaDateTime::now()'
        if quasar_data_type is None:
            return 'setNull{0}( OpcUa_StatusCode statusCode, {1})'.format(
                cap_first(name),
                source_time_stamp)
        else:
            return 'set{0}(const std::vector<{1}>& value, OpcUa_StatusCode statusCode, {2})'.format(cap_first(name), quasar_data_type, source_time_stamp)

    def getDelegatedWriteHeader(self, name, class_name, where):
        """ Returns the function header for a cache-variable write delegate.
                where  -- needs to be either body or header
                This function is modelled on its XSLT predecessor, in CommonFunctions, fnc:delegateWrite from quasar 1.3.12 """
        output = "UaStatus "
        if where == 'body':
            output += "AS" + class_name + "::"
        output += 'write{0}( Session* pSession, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel{1} )'.format(cap_first(name), ' = OpcUa_True' if where is 'header' else '')
        return output

    def fixDataTypePassingMethod(self, quasar_data_type, is_array):
        """ Modelled on CommonFunctions, fixDataTypePassingMethod
            TODO: this should be massively reviewed """
        if is_array:
            return 'const std::vector<{0}>& '.format(quasar_data_type)  ## TODO this most likely is wrong for e.g. UaString
        else: # scalar
            if quasar_data_type in ['UaString', 'UaByteString', 'UaVariant']:
                return 'const {0}& '.format(quasar_data_type)
            else:
                return quasar_data_type

    def quasarDataTypeToCppType(self, quasar_data_type, is_array):
        """ TODO: this needs to be cleaned-up, really """
        if is_array:
            return 'std::vector<{0}>'.format(quasar_data_type)
        else:
            return quasar_data_type

    def cacheVariableCppType(self, address_space_write, inWhichClass):
        if address_space_write in ['regular', 'forbidden']:
            return 'ChangeNotifyingVariable'
        elif address_space_write == 'delegated':
            return 'ASDelegatingVariable<AS{0}>'.format(inWhichClass)
        else:
            raise Exception("Unsupported address_space_write mode")

    def cacheVariableAccessLevel(self, address_space_write):
        if address_space_write in ['regular', 'forbidden']:
            return 'OpcUa_AccessLevels_CurrentRead'
        elif address_space_write == 'delegated':
            return 'OpcUa_AccessLevels_CurrentReadOrWrite'
        else:
            raise Exception("Unsupported address_space_write mode")

    def dataTypeToVariantSetter(self, quasar_data_type):
        if quasar_data_type == 'UaByteString':
            raise Exception('quasar logic error: UaByteString doesnt have a trivial (i.e. 1-arg) variant setter')
        else:
            return Oracle.DataTypeToVariantSetter[quasar_data_type]

    def dataTypeToVariantConverter(self, quasar_data_type):
        if quasar_data_type == 'UaString':
            raise Exception('quasar logic error: UaString has specific toString converter')
        else:
            return Oracle.DataTypeToVariantConverter[quasar_data_type]

    def dataTypeToBuiltinType(self, quasar_data_type):
        return Oracle.DataTypeToBuiltinType[quasar_data_type]

    def convertUaVariantToVectorFunctionName(self, quasar_data_type):
        if quasar_data_type == 'OpcUa_Byte':
            return 'ArrayTools::convertUaVariantToByteVector'
        elif quasar_data_type == 'OpcUa_Boolean':
            return 'ArrayTools::convertUaVariantToBooleanVector'
        else:
            return 'ArrayTools::convertUaVariantToVector'

    def convertVectorToUaVariantFunctionName(self, quasar_data_type):
        if quasar_data_type == 'OpcUa_Byte':
            return 'ArrayTools::convertByteVectorToUaVariant'
        elif quasar_data_type == 'OpcUa_Boolean':
            return 'ArrayTools::convertBooleanVectorToUaVariant'
        else:
            return 'ArrayTools::convertVectorToUaVariant'

    def wrapLiteral(self, quasar_data_type, literal):
        if quasar_data_type == 'UaString':
            return '"{0}"'.format(literal)
        else:
            return literal

    def sourceVariableReadAccessMask(self, address_space_read):
        if address_space_read in ['asynchronous', 'synchronous']:
            return 'OpcUa_AccessLevels_CurrentRead'
        else:
            return 0

    def sourceVariableWriteAccessMask(self, address_space_write):
        if address_space_write in ['asynchronous', 'synchronous']:
            return 'OpcUa_AccessLevels_CurrentWrite'
        else:
            return 0

    def sourceVariableReadJobId(self, class_name, variable_name, address_space_read):
        if address_space_read in ['asynchronous', 'synchronous']:
            return 'ASSOURCEVARIABLE_{0}_READ_{1}'.format(class_name, variable_name)
        else:
            return 'ASSOURCEVARIABLE_NOTHING'

    def sourceVariableWriteJobId(self, class_name, variable_name, address_space_write):
        if address_space_write in ['asynchronous', 'synchronous']:
            return 'ASSOURCEVARIABLE_{0}_WRITE_{1}'.format(class_name, variable_name)
        else:
            return 'ASSOURCEVARIABLE_NOTHING'

    def isDataTypeNumeric(self, quasar_data_type):
        if not quasar_data_type in Oracle.AllQuasarDataTypes:
            raise Exception('quasar_data_type {0} unknown'.format(quasar_data_type))
        return quasar_data_type in Oracle.NumericDataTypes