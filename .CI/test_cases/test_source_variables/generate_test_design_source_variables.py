# author: pnikiel
# date: 31-Mar-2020 

from Oracle import Oracle

def create_scenario_name(dataType, scalarArray, sync, rw, lock):
    return '{0}_{1}_{2}_{3}_{4}'.format(dataType.replace('_',''), scalarArray, sync, rw, lock)

f=open('Design.out', 'w')


def output(s):
    f.write(s+'\n')
    print(s)

def generate():
    for basicDataType in Oracle.AllQuasarDataTypes:
        for scalarArray in ['scalar']: # TODO @pnikiel forgotten that we have no arrays support for source-vars
            for sync in ['synchronous', 'asynchronous']:
                for rw in ['r', 'w', 'rw']:
                    for lock in ['no_mutex', 'mutex_this_operation', 'mutex_this_variable', 'mutex_containing_object']:
                        if sync == 'synchronous' and lock != 'no_mutex':
                            continue # that setting is excluded by quasar constraints
                        scenario_name = create_scenario_name(basicDataType, scalarArray, sync, rw, lock)
                        asWrite = 'forbidden' if 'w' not in rw else sync
                        asRead = 'forbidden' if 'r' not in rw else sync
                        QuasarLock = {
                            'no_mutex'                : 'no',
                            'mutex_this_operation'    : 'of_this_operation',
                            'mutex_this_variable'     : 'of_this_variable',
                            'mutex_containing_object' : 'of_containing_object'
                        }
                        output('<d:sourcevariable name="{0}" dataType="{1}" '
                            'addressSpaceWrite="{2}" addressSpaceRead="{3}" '
                            'addressSpaceReadUseMutex="{4}" addressSpaceWriteUseMutex="{4}" >'.format(
                            scenario_name,
                            basicDataType,
                            asWrite,
                            asRead,
                            QuasarLock[lock]))
                        if scalarArray == 'array':
                            output('<d:array/>')
                        output('</d:sourcevariable>')



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
