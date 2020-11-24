import sys

sys.path.insert(0, '../../../FrameworkInternals')

from Oracle import Oracle
from transform_filters import cap_first

def create_scenario_name(dataType, scalarArray):
	assert dataType in Oracle.AllQuasarDataTypes
	assert scalarArray in ['scalar', 'array']
	return '{0}_{1}'.format(dataType.replace('_',''), scalarArray)

f=open('Design.out', 'w')
f_devicelogic = open('DTestClass.cpp', 'w')

def output_devicelogic(s):
	f_devicelogic.write(s)
	f_devicelogic.write('\n')

def output(s):
	f.write(s)
	f.write('\n')
	print(s)


def generate():
	oracle = Oracle()
	# TODO: missing conditions: arrays and restrictions
	for scalarArray in ['scalar', 'array']:
		dataTypes = Oracle.InitializeFromConfigDataTypes
		for dataType in dataTypes:
			scenario_name = create_scenario_name(dataType, scalarArray)
			print(scenario_name)

			if scalarArray == 'scalar':
				output('<d:configentry name="{0}" dataType="{1}"/>'.format(
					scenario_name,
					dataType,
				))
			else:
				output('<d:configentry name="{0}" dataType="{1}">'.format(
					scenario_name,
					dataType,
				))
				output('<d:array/>')
				output('</d:configentry>')





output('<?xml version="1.0" encoding="UTF-8"?>')
output('<d:design xmlns:d="http://cern.ch/quasar/Design" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" projectShortName="TestProject" xsi:schemaLocation="http://cern.ch/quasar/Design Design.xsd">')
output('<d:class name="TestClass">')
generate()
output('</d:class>')
output('<d:root>')
output('<d:hasobjects instantiateUsing="configuration" class="TestClass"/>')
output('</d:root>')
output('</d:design>')
