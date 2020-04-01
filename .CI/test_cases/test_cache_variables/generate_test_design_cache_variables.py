from Oracle import Oracle

Initializers = ['configuration', 'valueAndStatus']
AddressSpaceWrites = ['forbidden', 'delegated']

SampleInitialValue = {
	'OpcUa_Boolean' : 'OpcUa_True',
	'UaString' : 'abcde'
}

for dataType in Oracle.InitializeFromValueAndStatusDataTypes:
	if dataType in Oracle.NumericDataTypes:
		SampleInitialValue[dataType] = '69'

def create_scenario_name(dataType, scalarArray, initializer, addressSpaceWriteScenario):
	assert dataType in Oracle.AllQuasarDataTypes
	assert scalarArray in ['scalar', 'array']
	assert initializer in Initializers
	assert addressSpaceWriteScenario in ['forbidden', 'delegated']
	return '{0}_{1}_{2}_{3}'.format(initializer, dataType.replace('_',''), scalarArray, addressSpaceWriteScenario)

def get_allowed_datatypes(initializer):
	assert initializer in Initializers
	if initializer == 'configuration':
		return Oracle.InitializeFromConfigurationDataTypes
	else:
		return Oracle.InitializeFromValueAndStatusDataTypes

f=open('Design.out', 'w')



def output(s):
	f.write(s)
	print(s)

def generate():
	for initializer in Initializers:
		for asWrite in AddressSpaceWrites:
			for scalarArray in ['scalar', 'array']:
				if initializer == 'valueAndStatus' and scalarArray == 'array':
					continue # this is not supported
				dataTypes = get_allowed_datatypes(initializer)
				for dataType in dataTypes:
					scenario_name = create_scenario_name(dataType, scalarArray, initializer, asWrite)
					print(scenario_name)

					if initializer == 'valueAndStatus':
						initialValue = SampleInitialValue[dataType]
						extra = 'initialStatus="OpcUa_Good" initialValue="{0}"'.format(initialValue)
					else:
						extra = ''
					if scalarArray == 'scalar':
						output('<d:cachevariable name="{0}" addressSpaceWrite="{1}" initializeWith="{2}" nullPolicy="nullForbidden" dataType="{3}" {4} />\n'.format(
							scenario_name,
							asWrite,
							initializer,
							dataType,
							extra
						))
					else:
						output('<d:cachevariable name="{0}" addressSpaceWrite="{1}" initializeWith="{2}" nullPolicy="nullForbidden" dataType="{3}" {4} >\n'.format(
							scenario_name,
							asWrite,
							initializer,
							dataType,
							extra
						))
						output('<d:array/>\n')
						output('</d:cachevariable>\n')


output('<?xml version="1.0" encoding="UTF-8"?>')
output('<d:design xmlns:d="http://cern.ch/quasar/Design" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" projectShortName="OpcUaSca" xsi:schemaLocation="http://cern.ch/quasar/Design Design.xsd">')
output('<d:class name="TestClass">')
output('<d:devicelogic/>')
generate()
output('</d:class>')
output('<d:root>')
output('<d:hasobjects instantiateUsing="configuration" class="TestClass"/>')
output('</d:root>')
output('</d:design>')