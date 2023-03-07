from Oracle import Oracle
import random

def create_scenario_name(num_args, num_rvs, it):
    return 'args{0}_rvs{1}_it{2}'.format(num_args, num_rvs, it)

f=open('Design.out', 'w')


def output(s):
    f.write(s)
    f.write('\n')
    print(s)



def make_dts():
    dts = []
    for basicDataType in Oracle.AllQuasarDataTypes:
        for scalarArray in ['scalar', 'array']:
            dts.append({'scalarArray':scalarArray, 'dataType':basicDataType})
    return dts



def get_dts(dts):
    #random.shuffle(dts)  #<-- not good for repetitivity of conditions
    return dts.pop()


def generate():
    # below is list of data types that we want to use for testing as types of arguments or return values
    # as scalars and arrays
    # None will be interpreted as no argument or no return value

    dts_args = make_dts()
    dts_rvs = make_dts()

    Num_arguments = [0, 1, 2]
    Num_return_values = [0, 1, 2]
    Random_Options = ['no', 'of_this_method', 'of_containing_object']
    it = 0
    try:
        while it<10:
            for num_arguments in Num_arguments:
                for num_return_values in Num_return_values:
                    scenario_name = create_scenario_name(num_arguments, num_return_values, it)
                    output(f'<d:method name="{scenario_name}" executionSynchronicity="asynchronous" addressSpaceCallUseMutex="{random.choice(Random_Options)}" >')
                    for arg in range(0, num_arguments):
                        dt = get_dts(dts_args)
                        output('<d:argument name="arg{0}" dataType="{1}" >'.format(arg, dt['dataType']) )
                        if dt['scalarArray'] == 'array':
                            output('<d:array/>')
                        output ('</d:argument>')
                    for rv in range(0, num_return_values):
                        dt = get_dts(dts_rvs)
                        output('<d:returnvalue name="rv{0}" dataType="{1}" >'.format(rv, dt['dataType']) )
                        if dt['scalarArray'] == 'array':
                            output('<d:array/>')
                        output ('</d:returnvalue>')
                    output('</d:method>')
            it += 1
    except IndexError:
        output('</d:method>')
        pass # this is expected, we just want to saturate the collection...



output('<?xml version="1.0" encoding="UTF-8"?>')
output('<d:design xmlns:d="http://cern.ch/quasar/Design" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" projectShortName="TestProject" xsi:schemaLocation="http://cern.ch/quasar/Design Design.xsd">')
output('<d:class name="TestClass">')
output('<d:devicelogic><d:mutex/></d:devicelogic>')
generate()
output('</d:class>')
output('<d:root>')
output('<d:hasobjects instantiateUsing="configuration" class="TestClass"/>')
output('</d:root>')
output('</d:design>')
