from Oracle import Oracle
from transform_filters import cap_first

Initializers = ['configuration', 'valueAndStatus']
AddressSpaceWrites = ['forbidden', 'delegated']
NullPolicy = ['nullForbidden', 'nullAllowed']

SampleInitialValue = {
	'OpcUa_Boolean' : 'OpcUa_True',
	'UaString' : 'abcde'
}

for dataType in Oracle.ValueAndStatusInitDataTypes:
	if dataType in Oracle.NumericDataTypes:
		SampleInitialValue[dataType] = '69'

def create_scenario_name(dataType, scalarArray, initializer, addressSpaceWriteScenario, nullPolicy):
	assert dataType in Oracle.AllQuasarDataTypes
	assert scalarArray in ['scalar', 'array']
	assert initializer in Initializers
	assert addressSpaceWriteScenario in ['forbidden', 'delegated']
	return '{0}_{1}_{2}_{3}_{4}'.format(initializer, dataType.replace('_',''), scalarArray, addressSpaceWriteScenario, nullPolicy)

def get_allowed_datatypes(initializer):
	assert initializer in Initializers
	if initializer == 'configuration':
		return Oracle.InitializeFromConfigDataTypes
	else:
		return Oracle.ValueAndStatusInitDataTypes

f=open('Design.out', 'w')
f_devicelogic = open('DTestClass.cpp', 'w')

def output_devicelogic(s):
	f_devicelogic.write(s)
	f_devicelogic.write('\n')

def output(s):
	f.write(s)
	print(s)

def generateDeviceLogicCase (dataType, scalarArray, initializer, asWrite, nullPolicy):
	scenario_name = create_scenario_name(dataType, scalarArray, initializer, asWrite, nullPolicy)
	output_devicelogic('{{ // scenario name: {0} '.format(scenario_name))
	if dataType != 'UaVariant':
		sampleInitialValue = SampleInitialValue[dataType]
		if dataType == 'UaString':
			sampleInitialValue = '"{0}"'.format(sampleInitialValue)
		if scalarArray == 'array':
			sampleInitialValue = '{{ {0} }}'.format(sampleInitialValue)
		sampleInitialValue = "({0})".format(sampleInitialValue)
	else:
		sampleInitialValue = ''
	cppType = dataType if scalarArray == 'scalar' else 'std::vector<{0}>'.format(dataType)
	output_devicelogic('{0} test_value {1};'.format(cppType, sampleInitialValue))
	output_devicelogic('getAddressSpaceLink()->set{0}(test_value, OpcUa_Good);'.format(cap_first(scenario_name)))
	output_devicelogic('getAddressSpaceLink()->get{0}(test_value);'.format(cap_first(scenario_name)))
	if nullPolicy == 'nullForbidden':
		output_devicelogic('test_value = getAddressSpaceLink()->get{0}();'.format(cap_first(scenario_name)))
	if nullPolicy == 'nullAllowed':
		output_devicelogic('getAddressSpaceLink()->setNull{0}(OpcUa_Good);'.format(cap_first(scenario_name)))
	output_devicelogic('}} // scenario name: {0} '.format(scenario_name))

def generate():
	oracle = Oracle()
	for initializer in Initializers:
		for asWrite in AddressSpaceWrites:
			for scalarArray in ['scalar', 'array']:
				for nullPolicy in NullPolicy:
					if initializer == 'valueAndStatus' and scalarArray == 'array':
						continue # this is not supported
					dataTypes = get_allowed_datatypes(initializer)
					for dataType in dataTypes:
						scenario_name = create_scenario_name(dataType, scalarArray, initializer, asWrite, nullPolicy)
						print(scenario_name)

						if initializer == 'valueAndStatus' and dataType == 'UaVariant' and nullPolicy == 'nullForbidden':
							continue # this setting is clearly invalid!
							
						generateDeviceLogicCase(dataType, scalarArray, initializer, asWrite, nullPolicy)
						
						if initializer == 'valueAndStatus':
							if dataType != 'UaVariant':		
								initialValue = SampleInitialValue[dataType]
								extra = 'initialStatus="OpcUa_Good" initialValue="{0}"'.format(initialValue)
							else:
								extra = 'initialStatus="OpcUa_Good" ' # no initial value!
						else:
							extra = ''
						if scalarArray == 'scalar':
							output('<d:cachevariable name="{0}" addressSpaceWrite="{1}" initializeWith="{2}" nullPolicy="{5}" dataType="{3}" {4} />\n'.format(
								scenario_name,
								asWrite,
								initializer,
								dataType,
								extra,
								nullPolicy
							))
						else:
							output('<d:cachevariable name="{0}" addressSpaceWrite="{1}" initializeWith="{2}" nullPolicy="{5}" dataType="{3}" {4} >\n'.format(
								scenario_name,
								asWrite,
								initializer,
								dataType,
								extra,
								nullPolicy
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
