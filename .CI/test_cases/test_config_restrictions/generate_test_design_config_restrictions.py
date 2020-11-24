#!/usr/bin/env python3
# encoding: utf-8
'''
generate_test_design_config_restrictions.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

@license:
Copyright (c) 2020, CERN.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
   and the following disclaimer in the documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS  OR
IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED WARRANTIES  OF  MERCHANTABILITY
AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
DAMAGES (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR  SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY  OF  LIABILITY,
WHETHER IN  CONTRACT, STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN
ANY WAY OUT OF  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import sys

sys.path.insert(0, '../../../FrameworkInternals')

from Oracle import Oracle
from transform_filters import cap_first

def create_scenario_name(what, dataType, scalarArray):
    assert dataType in Oracle.AllQuasarDataTypes
    assert scalarArray in ['scalar', 'array']
    return '{0}_{1}_{2}'.format(what, dataType.replace('_',''), scalarArray)

f=open('Design.out', 'w')

def output_devicelogic(s):
    f_devicelogic.write(s)
    f_devicelogic.write('\n')

def output(s):
    f.write(s)
    f.write('\n')
    print(s)

def generate():
    oracle = Oracle()
    for what in ['configentry', 'cachevariable']:
        for scalarArray in ['scalar', 'array']:
            dataTypes = Oracle.InitializeFromConfigDataTypes
            for dataType in dataTypes:
                scenario_name = create_scenario_name(what, dataType, scalarArray)
                print(scenario_name)
                output('<d:{0} name="{1}" dataType="{2}" '.format(
                        what,
                        scenario_name,
                        dataType,
                        ))
                if what == "cachevariable":
                    output('initializeWith="configuration" nullPolicy="nullForbidden" addressSpaceWrite="forbidden" ')
                output('>')
                if scalarArray == 'array':
                    output('<d:array/>')

                output('<d:configRestriction>')
                # for integer data types we can easily "invent" the restriction to be just a bit tighter
                # that nominal numeric limits
                if dataType in oracle.IntegerDataTypesRange:
                    range_tuple = oracle.IntegerDataTypesRange[dataType]
                    output('<d:restrictionByBounds minInclusive="{0}" maxInclusive="{1}"/>'.format(range_tuple[0]+1, range_tuple[1]-1))
                elif dataType in ['OpcUa_Double', 'OpcUa_Float']:
                    output('<d:restrictionByBounds minInclusive="-12345678.999" maxInclusive="1234567890.999"/>')
                elif dataType in ['UaString']:
                    output('<d:restrictionByEnumeration>')
                    output('<d:enumerationValue value="Parting is such sweet sorrow"/>')
                    output('<d:enumerationValue value="That I shall say good night till it be morrow."/>')
                    output('</d:restrictionByEnumeration>')
                output('<d:restrictionByPattern pattern=".*"/>')
                output('</d:configRestriction>')
                output('</d:{0}>'.format(what))


def main():
    output('<?xml version="1.0" encoding="UTF-8"?>')
    output('<d:design xmlns:d="http://cern.ch/quasar/Design" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" projectShortName="TestProject" xsi:schemaLocation="http://cern.ch/quasar/Design Design.xsd">')
    output('<d:class name="TestClass">')
    generate()
    output('</d:class>')
    output('<d:root>')
    output('<d:hasobjects instantiateUsing="configuration" class="TestClass"/>')
    output('</d:root>')
    output('</d:design>')

if __name__ == "__main__":
    main()
