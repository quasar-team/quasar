#!/usr/bin/env python3
# encoding: utf-8
'''
Oracle.py

@author:     Piotr Nikiel <piotr@nikiel.info>
@author:     Ben Farnham  <firstNm.secondNm@cern.ch>

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
    """This class represents knowledge about quasar architecture.
    It knows how to map quasar data types into address-space or device-logic
    C++ types, how to map that into node ids, XSD types, etc."""

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
    InitializeFromConfigDataTypes = [
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

    ValueAndStatusInitDataTypes = [
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

    QuasarTypeToXsdType = {
        'UaString'      : 'xs:string',
        'OpcUa_SByte'   : 'xs:byte',
        'OpcUa_Byte'    : 'xs:unsignedByte',
        'OpcUa_Int16'   : 'xs:short',
        'OpcUa_UInt16'  : 'xs:unsignedShort',
        'OpcUa_Int32'   : 'xs:int',
        'OpcUa_UInt32'  : 'xs:unsignedInt',
        'OpcUa_Int64'   : 'xs:long',
        'OpcUa_UInt64'  : 'xs:unsignedLong',
        'OpcUa_Boolean' : 'xs:boolean',
        'OpcUa_Double'  : 'xs:double',
        'OpcUa_Float'   : 'xs:float'
    }

    IntegerDataTypesRange = {
        'OpcUa_SByte'   : (-2**7, 2**7-1),
        'OpcUa_Byte'    : (0, 2**8-1),
        'OpcUa_Int16'   : (-2**15, 2**15-1),
        'OpcUa_UInt16'  : (0, 2**16-1),
        'OpcUa_Int32'   : (-2**31, 2**31-1),
        'OpcUa_UInt32'  : (0, 2**31-1),
        'OpcUa_Int64'   : (-2**63, 2**63-1),
        'OpcUa_UInt64'  : (0, 2**63-1)
    }

    def data_type_to_device_type(self, quasar_data_type):
        """TODO @pnikiel We will clean-up type derivation in subsequent efforts """
        if quasar_data_type == "UaString":
            return "std::string"
        else:
            return quasar_data_type

    def get_cache_variable_setter_args(self, quasar_data_type, for_header, new_style_null=False):
        """Returns the arguments part of SCALAR! cache variable setter
           TODO @pnikiel: new_style_null at some point should replace the old-style null.
            """
        source_time_stamp = 'const UaDateTime& srcTime'
        if for_header:
            source_time_stamp += '= UaDateTime::now()'
        if quasar_data_type in Oracle.PassByValueDataTypes:
            return f'{quasar_data_type} value, OpcUa_StatusCode statusCode, {source_time_stamp}'
        elif quasar_data_type is None:  # formerly null
            if new_style_null:
                return f'QuasarNullDataType null, OpcUa_StatusCode statusCode, {source_time_stamp}'
            else: # this branch to be removed in one of next releases, TODO.
                return f'OpcUa_StatusCode statusCode, {source_time_stamp}'
        else:
            return f'const {quasar_data_type}& value, OpcUa_StatusCode statusCode, {source_time_stamp}'

    def get_cache_variable_setter(self, name, quasar_data_type, for_header, new_style_null=False):
        """This function is based on its XSLT version, CommonFunctions,
           fnc:varSetter from quasar 1.3.12
           TODO @pnikiel should be reworked to profit from other functions
           TODO @pnikiel: new_style_null at some point should replace the old-style null.
            """
        if quasar_data_type is None and not new_style_null: #old-style Null
            return f'setNull{cap_first(name)}({self.get_cache_variable_setter_args(quasar_data_type, for_header, new_style_null)})'
        else:
            return f'set{cap_first(name)}({self.get_cache_variable_setter_args(quasar_data_type, for_header, new_style_null)})'

    def get_cache_variable_setter_array(self, name, quasar_data_type, for_header, new_style_null=False):
        """ TODO: description """
        source_time_stamp = 'const UaDateTime& srcTime'
        if for_header:
            source_time_stamp += '= UaDateTime::now()'
        if quasar_data_type is None:
            if new_style_null:
                return ('set{0}( QuasarNullDataType null, '
                        'OpcUa_StatusCode statusCode, {1})').format(
                            cap_first(name), source_time_stamp)
            else:
                return 'setNull{0}( OpcUa_StatusCode statusCode, {1})'.format(
                    cap_first(name),
                    source_time_stamp)
        else:
            return ('set{0}(const std::vector<{1}>& value, '
                    'OpcUa_StatusCode statusCode, {2})').format(
                        cap_first(name), quasar_data_type, source_time_stamp)

    def get_delegated_write_header(self, name, class_name, where):
        """ Returns the function header for a cache-variable write delegate.
        where  -- needs to be either body or header
        This function is modelled on its XSLT predecessor, in CommonFunctions,
        fnc:delegateWrite from quasar 1.3.12 """
        output = "UaStatus "
        if where == 'body':
            output += "AS" + class_name + "::"
        initializer = ' = OpcUa_True' if where == 'header' else ''
        output += ('write{0}( Session* pSession, const UaDataValue& dataValue,'
                   ' OpcUa_Boolean checkAccessLevel{1} )').format(
                       cap_first(name), initializer)
        return output

    def fix_data_type_passing_method(self, quasar_data_type, is_array):
        """ Modelled on CommonFunctions, fix_data_type_passing_method
            TODO: this should be massively reviewed """
        if is_array:
            ## TODO this most likely is wrong for e.g. UaString
            return 'const std::vector<{0}>& '.format(quasar_data_type)
        else: # scalar
            if quasar_data_type in ['UaString', 'UaByteString', 'UaVariant']:
                return 'const {0}& '.format(quasar_data_type)
            else:
                return quasar_data_type

    def quasar_data_type_to_cpp_type(self, quasar_data_type, is_array):
        """ TODO: this needs to be cleaned-up, really """
        if is_array:
            return 'std::vector<{0}>'.format(quasar_data_type)
        else:
            return quasar_data_type

    def cache_variable_cpp_type(self, address_space_write, in_which_class, is_array=True):
        """Returns C++ class to represent this cache-var in address-space"""
        if is_array:
            if address_space_write in ['regular', 'delegated']:
                #note @piotr: the reason for having 'regular' routed via delegating variable is that
                #we have to do size validation.
                return 'ASDelegatingVariable<AS{0}>'.format(in_which_class)
            else:
                #note @piotr: it could be BaseDataVariableType as well, because for the moment
                #there is no way to run CalculatedVariable on ChangeNotifyingVariable
                return 'ChangeNotifyingVariable'
        else:
            if address_space_write in ['regular', 'forbidden']:
                return 'ChangeNotifyingVariable'
            elif address_space_write == 'delegated':
                return 'ASDelegatingVariable<AS{0}>'.format(in_which_class)
            else:
                raise Exception("Unsupported address_space_write mode")

    def cache_variable_access_level(self, address_space_write):
        """Returns value of OPC-UA accessLevel attribute"""
        if address_space_write == 'forbidden':
            return 'OpcUa_AccessLevels_CurrentRead'
        elif address_space_write in ['regular', 'delegated']:
            return 'OpcUa_AccessLevels_CurrentReadOrWrite'
        else:
            raise Exception("Unsupported address_space_write mode")

    def data_type_to_variant_setter(self, quasar_data_type):
        """Returns name of method of UaVariant that sets given data type"""
        if quasar_data_type == 'UaByteString':
            raise Exception(('quasar logic error: UaByteString doesnt have a '
                             'trivial (i.e. 1-arg) variant setter'))
        else:
            return Oracle.DataTypeToVariantSetter[quasar_data_type]

    def data_type_to_variant_converter(self, quasar_data_type):
        """Returns name of method of UaVariant that converts to the data type"""
        if quasar_data_type == 'UaString':
            raise Exception(
                'quasar logic error: UaString has specific toString converter')
        else:
            return Oracle.DataTypeToVariantConverter[quasar_data_type]

    def data_type_to_builtin_type(self, quasar_data_type):
        """Returns a numeric constant which represents given type in the
           OPC-UA information model"""
        return Oracle.DataTypeToBuiltinType[quasar_data_type]

    def uavariant_to_vector_function(self, quasar_data_type):
        """Returns the name of function from ArrayTools which can perform
           that conversion"""
        if quasar_data_type == 'OpcUa_Byte':
            return 'ArrayTools::convertUaVariantToByteVector'
        elif quasar_data_type == 'OpcUa_Boolean':
            return 'ArrayTools::convertUaVariantToBooleanVector'
        else:
            return 'ArrayTools::convertUaVariantToVector'

    def vector_to_uavariant_function(self, quasar_data_type):
        """Returns the name of function from ArrayTools which can perform
           that conversion"""
        if quasar_data_type == 'OpcUa_Byte':
            return 'ArrayTools::convertByteVectorToUaVariant'
        elif quasar_data_type == 'OpcUa_Boolean':
            return 'ArrayTools::convertBooleanVectorToUaVariant'
        else:
            return 'ArrayTools::convertVectorToUaVariant'

    def wrap_literal(self, quasar_data_type, literal):
        """For string constants wraps in the double-quotes as C++ literals"""
        if quasar_data_type == 'UaString':
            return '"{0}"'.format(literal)
        else:
            return literal

    def source_var_read_access_mask(self, address_space_read):
        """Returns OPC-UA access level mask"""
        if address_space_read in ['asynchronous', 'synchronous']:
            return 'OpcUa_AccessLevels_CurrentRead'
        else:
            return 0

    def source_var_write_access_mask(self, address_space_write):
        """Returns OPC-UA access level mask"""
        if address_space_write in ['asynchronous', 'synchronous']:
            return 'OpcUa_AccessLevels_CurrentWrite'
        else:
            return 0

    def source_var_read_job_id(self, class_name, variable_name, address_space_read):
        """Returns job id for given source variable"""
        if address_space_read in ['asynchronous', 'synchronous']:
            return 'ASSOURCEVARIABLE_{0}_READ_{1}'.format(class_name, variable_name)
        else:
            return 'ASSOURCEVARIABLE_NOTHING'

    def source_var_write_job_id(self, class_name, variable_name, address_space_write):
        """Returns job id for given source variable"""
        if address_space_write in ['asynchronous', 'synchronous']:
            return 'ASSOURCEVARIABLE_{0}_WRITE_{1}'.format(class_name, variable_name)
        else:
            return 'ASSOURCEVARIABLE_NOTHING'

    def is_data_type_numeric(self, quasar_data_type):
        """Returns True if given type can be considered numeric"""
        if not quasar_data_type in Oracle.AllQuasarDataTypes:
            raise Exception('quasar_data_type {0} unknown'.format(quasar_data_type))
        return quasar_data_type in Oracle.NumericDataTypes

    def quasar_data_type_to_xsd_type(self, quasar_data_type):
        if not quasar_data_type in Oracle.AllQuasarDataTypes:
            raise Exception('quasar_data_type {0} unknown'.format(quasar_data_type))
        if not quasar_data_type in Oracle.QuasarTypeToXsdType:
            raise Exception(
                "This quasar type {0} cant't be initialized from XSD configuration".format(
                    quasar_data_type
                ))
        return Oracle.QuasarTypeToXsdType[quasar_data_type]

    def classify_xsd_restrictions(self, xsd_restrictions):
        """xsd_restrictions should be a list of tuples in which first element is XSD restriction
           type (pattern, minInclusive, ...) and the 2nd element is the XSD restriction value

           Returns None if the classification can't be done (e.g. mixed types)"""

        if len(xsd_restrictions) < 1:
            return None
        RESTRICTIONS = {
            'bounds' : ('minInclusive', 'maxInclusive'),
            'pattern' : ('pattern',),
            'enumeration' : ('enumeration',)
            }
        for klass in RESTRICTIONS:
            if all([element[0] in RESTRICTIONS[klass] for element in xsd_restrictions]):
                return klass
        return None
