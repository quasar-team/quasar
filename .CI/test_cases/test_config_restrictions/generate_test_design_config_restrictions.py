# TODO: file header.

import sys

sys.path.insert(0, '../../../FrameworkInternals')

from Oracle import Oracle
from transform_filters import cap_first

def create_scenario_name(dataType, scalarArray):
    assert dataType in Oracle.AllQuasarDataTypes
    assert scalarArray in ['scalar', 'array']
    return '{0}_{1}'.format(dataType.replace('_',''), scalarArray)

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
    for scalarArray in ['scalar', 'array']:
        dataTypes = Oracle.InitializeFromConfigDataTypes
        for dataType in dataTypes:
            scenario_name = create_scenario_name(dataType, scalarArray)
            print(scenario_name)
            output('<d:configentry name="{0}" dataType="{1}">'.format(
                    scenario_name,
                    dataType,
                    ))
            if scalarArray == 'array':
                output('<d:array/>')

            output('<d:configRestriction>')
            # for integer data types we can easily "invent" the restriction to be just a bit tighter
            # that nominal numeric limits
            if dataType in oracle.IntegerDataTypesRange:
                range_tuple = oracle.IntegerDataTypesRange[dataType]
                output('<d:restrictionByBounds minInclusive="{0}" maxInclusive="{1}"/>'.format(range_tuple[0]+1, range_tuple[1]-1))
            output('<d:configRestriction/>')
            output('</d:configentry>')


def main():
    output('<?xml version="1.0" encoding="UTF-8"?>')
    output('<d:design xmlns:d="http://cern.ch/quasar/Design" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" projectShortName="OpcUaSca" xsi:schemaLocation="http://cern.ch/quasar/Design Design.xsd">')
    output('<d:class name="TestClass">')
    generate()
    output('</d:class>')
    output('<d:root>')
    output('<d:hasobjects instantiateUsing="configuration" class="TestClass"/>')
    output('</d:root>')
    output('</d:design>')

if __name__ == "__main__":
    main()
