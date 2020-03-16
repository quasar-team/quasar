#!/usr/bin/env python
# encoding: utf-8
'''
Oracle.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

@license:
Copyright (c) 2015, CERN.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import pdb # TODO remove

def capFirst(s):
    return s[0].upper() + s[1:]

class Oracle():
	PassByValueDataTypes = [
		'OpcUa_Boolean',
		'OpcUa_UInt32',
		'OpcUa_UInt64',
		'OpcUa_Double',
		'OpcUa_Float']

	def getDeviceLogicTypeFromQuasarType(self, t):
		if t == "UaString":
			return "std::string"
		else:
			return t

	def getCacheVariableSetter(self, name, dataType, forHeader):
		""" This function is based on its XSLT version, CommonFunctions, fnc:varSetter from quasar 1.3.12 """
		""" TODO: should use whatIsDeviceLogicDataType """
		srcTimeStamp = 'const UaDateTime& srcTime'
		if forHeader:
			srcTimeStamp += '= UaDateTime::now()'
		if dataType in Oracle.PassByValueDataTypes:
			return 'set{0}( {1} value, OpcUa_StatusCode statusCode, {2} )'.format(capFirst(name), dataType, srcTimeStamp)
		elif dataType is None:  # formerly null
			return 'setNull{0}( OpcUa_StatusCode statusCode, {1})'.format(capFirst(name), srcTimeStamp)
		else:
			return 'set{0}( const {1}& value, OpcUa_StatusCode statusCode, {2})'.format(capFirst(name), dataType, srcTimeStamp)

	def getCacheVariableSetterArray(self, name, dataType, forHeader):
		""" TODO: description """
		srcTimeStamp = 'const UaDateTime& srcTime'
		if forHeader:
			srcTimeStamp += '= UaDateTime::now()'
		if dataType is None:
			return 'setNull{0}( OpcUa_StatusCode statusCode, {1})'.format(capFirst(name), srcTimeStamp)
		else:
			return 'set{0}(const std::vector<{1}>& value, OpcUa_StatusCode statusCode, {2})'.format(capFirst(name), dataType, srcTimeStamp) 

	def getDelegatedWriteHeader(self, name, className, where):
		""" Returns the function header for a cache-variable write delegate.
			where  -- needs to be either body or header
			This function is modelled on its XSLT predecessor, in CommonFunctions, fnc:delegateWrite from quasar 1.3.12 """
		output = "UaStatus "
		if where is 'body':
			output += className + "::"
		output += 'write{0}( Session* pSession, const UaDataValue& dataValue, OpcUa_Boolean checkAccessLevel{1} )'.format(capFirst(name), ' = OpcUa_True' if where is 'body' else '')
		return output

	def fixDataTypePassingMethod(self, quasarDataType, isArray):
		""" Modelled on CommonFunctions, fixDataTypePassingMethod"""
		""" TODO: this should be massively reviewed """
		if isArray:
			return 'const std::vector<{0}>& '.format(quasarDataType)  ## TODO this most likely is wrong for e.g. UaString
		else: # scalar
			if quasarDataType in ['UaString', 'UaByteString', 'UaVariant']:
				return 'const {0}& '.format(quasarDataType)
			else:
				return quasarDataType

	def quasarDataTypeToCppType(self, quasarDataType, isArray):
		""" TODO: this needs to be cleaned-up, really """
		if isArray:
			return 'std::vector<{0}>'.format(quasarDataType)
		else:
			return quasarDataType
