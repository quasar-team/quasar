#!/usr/bin/env python3
# encoding: utf-8
'''
DesignValidator.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

@license:
Copyright (c) 2015, CERN.
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

Note(pnikiel): quasar design validation is composed of two stages:
- 1. making sure it conforms to Design.xsd,
- 2. validating certain parts that are difficult/impractical/impossible by XSD.
Up to quasar 1.4.0 stage 1 was by executing xmllint and stage 2 was by running a XSLT transform.
Since quasar 1.4.1 both stages are in this class, stage 1 is validate_first_stage and stage 2 is
validate_second_stage. Running validate will invoke both stages.

'''

import io
from colorama import Fore, Style
from lxml import etree
from quasarExceptions import DesignFlaw
import DesignInspector
from Oracle import Oracle

def stringify_locator(locator):
    """Creates comma-separated representation of the dictionary in another font color. It is to
       point out where (in which class, variable, etc) given problem appeared"""
    locator_as_list = ["{0}={1}".format(key, locator[key]) for key in locator]
    return ', '.join(locator_as_list)

def assert_attribute_absent(element, attribute, extra_msg, locator):
    """Throws if attribute is absent"""
    if attribute in element.attrib:
        raise DesignFlaw("{0} attribute can't be present {1} (at: {2})".format(
            attribute, extra_msg, stringify_locator(locator)))

def assert_attribute_present(element, attribute, extra_msg, locator):
    """Throws if attribute is present"""
    if not attribute in element.attrib:
        raise DesignFlaw("{0} attribute must be present {1} (at: {2})".format(
            attribute, extra_msg, stringify_locator(locator)))

def assert_attribute_equal(element, attribute, expected_value, extra_msg, locator):
    """Throws if attribute not equal"""
    if element.get(attribute) != expected_value:
        raise DesignFlaw("Attribute {0} expects {1} {2} (at: {3})".format(
            attribute, expected_value, extra_msg, stringify_locator(locator)))

def count_children(parent, element_name):
    """Supports potentially absent elements where trivial counting by len(parent.element_name) would
       throw because of failed lookup."""
    count = 0
    try:
        count = len(parent.__getattr__(element_name))
    except AttributeError:
        pass  # that's OK, it's simply not there
    return count

def assert_numeric_literal_valid(literal, quasar_data_type, locator):
    """Will throw if given literal is out of numeric limits"""
    try:
        if quasar_data_type in ['OpcUa_Float', 'OpcUa_Double']:
            test_f = float(literal)
        else:
            test = int(literal)
            valid_range = Oracle.IntegerDataTypesRange[quasar_data_type]
            if test < valid_range[0] or test > valid_range[1]:
                raise DesignFlaw("literal '{0}' not in integer valid_range {1} (at: {2})".format(
                    literal, valid_range, stringify_locator(locator)))
    except ValueError:
        raise DesignFlaw("literal '{0}' unparsable for data type '{1}' (at: {2})".format(
            literal, quasar_data_type, stringify_locator(locator)))

class DesignValidator():
    """quasar design file validator"""
    def __init__(self, schema_file_path, design_file_path):
        schema_file = io.open(schema_file_path, 'r', encoding='utf-8')
        schema_doc = etree.parse(schema_file)
        self.schema = etree.XMLSchema(schema_doc)
        self.design_inspector = DesignInspector.DesignInspector(design_file_path)

    def validate(self):
        """Performs both stage 1 and stage 2 validation. Should be used by default. Throws if design
           is not valid"""
        self.validate_first_stage()
        self.validate_second_stage()
        print(Fore.GREEN + 'Your Design is valid.' + Style.RESET_ALL)

    def validate_first_stage(self):
        """Performs only stage 1 validation. Internal method"""
        self.schema.assertValid(self.design_inspector.tree)  # will throw if invalid

    def validate_second_stage(self):
        """Performs only stage 2 validation. Internal method"""
        self.validate_classes()
        self.validate_cache_variables()
        self.validate_source_variables()
        self.validate_config_entries()
        self.validate_hasobjects_wrapper()
        self.validate_methods()

    def validate_initial_value(self, cachevariable, locator):
        """initialValue is there, but its format depends on dataType so can't be validated by XSD"""
        initial_value = cachevariable.get('initialValue')
        data_type = cachevariable.get('dataType')
        if data_type == 'UaString':
            return  # any text is valid for string
        if data_type == 'OpcUa_Boolean':
            valid_choices = ['OpcUa_True', 'OpcUa_False']
            if initial_value not in valid_choices:
                raise DesignFlaw('initialValue wrong literal, only {0} allowed (at: {1})'.format(
                    ' or '.join(valid_choices), stringify_locator(locator)))
        elif data_type in Oracle.NumericDataTypes:
            assert_numeric_literal_valid(initial_value, data_type, locator)
        else:
            pass  # UaByteString and UaVariant are already excluded as initialValue must be absent

    def validate_array(self, array, locator):
        """Perfoms validation of objectified d:array element"""
        if 'minimumSize' in array.attrib and 'maximumSize' in array.attrib:
            if array.get('minimumSize') > array.get('maximumSize'):
                raise DesignFlaw("array's minimumSize > maximumSize, fix it."
                                 + stringify_locator(locator))


    def validate_classes(self):
        """Performs validation of all classes in the design"""
        for class_name in self.design_inspector.get_names_of_all_classes():
            locator = {'class':class_name}
            cls = self.design_inspector.objectify_class(class_name)
            locator['line_num'] = cls.sourceline
            if self.design_inspector.is_class_single_variable_node(class_name):
                # assert that single_variable_node needs to have precisely one variable or method
                count_cachevars = count_children(cls, 'cachevariable')
                count_sourcevars = count_children(cls, 'sourcevariable')
                count_methods = count_children(cls, 'method')
                address_space_nodes_cnt = count_cachevars + count_sourcevars + count_methods
                if address_space_nodes_cnt != 1:
                    raise DesignFlaw(("class is singleVariableNode but has {0} address space "
                                      "children instead of one. (at: {1})").format(
                                          str(address_space_nodes_cnt), stringify_locator(locator)))
                # assert that single_variable_node can't have any hasobjects
                has_objects_count = count_children(cls, 'hasobjects')
                if has_objects_count > 0:
                    raise DesignFlaw(("class is singleVariableNode but has {0} hasobjects, should"
                                      "have none. (at: {1})").format(
                                          str(len(has_objects_count)), stringify_locator(locator)))
            if self.design_inspector.get_class_default_instance_name(class_name) is not None:
                if not self.design_inspector.is_class_always_singleton(class_name):
                    raise DesignFlaw(("defaultInstanceName can only be used with singleton "
                                      f"classes (at class {class_name})"))

    def validate_cache_variables(self):
        """Performs validation of all cache variables in the design"""
        for class_name in self.design_inspector.get_names_of_all_classes():
            locator = {'class':class_name}
            cls = self.design_inspector.objectify_class(class_name)
            if count_children(cls, 'cachevariable') == 0:
                continue
            for cache_variable in cls.cachevariable:
                locator['cachevariable'] = cache_variable.get('name')
                locator['line_num'] = cache_variable.sourceline
                if cache_variable.get('initializeWith') == 'configuration':
                    assert_attribute_absent(cache_variable, 'initialValue',
                                            'when initializeWith=configuration', locator)
                    assert_attribute_absent(cache_variable, 'initialStatus',
                                            'when initializeWith=configuration', locator)
                if cache_variable.get('initializeWith') == 'valueAndStatus':
                    assert_attribute_present(cache_variable, 'initialStatus',
                                             'when initializeWith=valueAndStatus', locator)
                    if cache_variable.get('nullPolicy') == 'nullForbidden':
                        assert_attribute_present(cache_variable, 'initialValue',
                                                 'when valueAndStatus and nullForbidden', locator)
                if cache_variable.get('initialValue') is not None:
                    self.validate_initial_value(cache_variable, locator)
                if count_children(cache_variable, 'array') > 0:
                    self.validate_array(cache_variable.array, locator)
                    assert_attribute_absent(cache_variable, 'initialValue',
                                            'when array', locator)
                    assert_attribute_absent(cache_variable, 'defaultConfigInitializerValue',
                                            'when array', locator)
                if cache_variable.get('dataType') in ['UaVariant', 'UaByteString']:
                    assert_attribute_equal(cache_variable, 'initializeWith', 'valueAndStatus',
                                           'when data type is UaVariant', locator)
                    assert_attribute_absent(cache_variable, 'initialValue',
                                            'when data type is UaVariant', locator)

    def assert_mutex_present(self, class_name, locator, extra_info=''):
        """Raises DesignFlaw if class 'class_name' doesnt have a mutex"""

        if not self.design_inspector.class_has_device_logic(class_name):
            raise DesignFlaw('Class {2} needs device-logic to have a mutex (at: {0}) {1}'.format(
                stringify_locator(locator), extra_info, class_name))
        cls = self.design_inspector.objectify_class(class_name)
        if count_children(cls.devicelogic, 'mutex') < 1:
            raise DesignFlaw('Class {2} needs a mutex in its device logic(at: {0}) {1}'.format(
                stringify_locator(locator), extra_info, class_name))

    def validate_source_variables(self):
        for class_name in self.design_inspector.get_names_of_all_classes():
            locator = {'class':class_name}
            cls = self.design_inspector.objectify_class(class_name)
            for source_variable in self.design_inspector.objectify_source_variables(class_name):
                locator['sourcevariable'] = source_variable.get('name')
                if source_variable.get('addressSpaceRead') == 'synchronous' and source_variable.get('addressSpaceReadUseMutex') != 'no':
                    raise DesignFlaw(f'Cant use synchro domains (...ReadUseMutex attribute) for (network-)synchro'
                        f' execution (at {stringify_locator(locator)})')
                if source_variable.get('addressSpaceWrite') == 'synchronous' and source_variable.get('addressSpaceWriteUseMutex') != 'no':
                    raise DesignFlaw(f'Cant use synchro domains (...WriteUseMutex attribute) for (network-)synchro'
                        f' execution (at {stringify_locator(locator)})')
                mutex_options = [
                    source_variable.get('addressSpaceReadUseMutex'),
                    source_variable.get('addressSpaceWriteUseMutex')]
                # remove duplicates
                mutex_options = list(set(mutex_options))
                # remove values which don't require inter-class sync, thus need no validation
                mutex_options = [x for x in mutex_options if x not in [
                    'no', 'of_this_operation', 'of_this_variable', 'handpicked']]
                for option in mutex_options:
                    if option == 'of_containing_object':
                        self.assert_mutex_present(class_name, locator,
                                                  'to support setting "{0}"'.format(option))
                    elif option == 'of_parent_of_containing_object':
                        parent = self.design_inspector.get_parent(class_name)
                        if parent is None:
                            raise DesignFlaw(('Class {0} has no unique parent, cant use "{1}" '
                                              '(at: {2})').format(
                                                  class_name, option, stringify_locator(locator)))
                        self.assert_mutex_present(parent, locator, 'to support setting "{0}"'.format(
                            option))
                    else:
                        raise NotImplementedError("Don't know how to validate '{0}'".format(option))


    def validate_config_entries(self):
        """Performs validation of all config entries in the design"""
        for class_name in self.design_inspector.get_names_of_all_classes():
            locator = {'class':class_name}
            cls = self.design_inspector.objectify_class(class_name)
            if count_children(cls, 'configentry') == 0:
                continue
            for config_entry in cls.configentry:
                locator['configentry'] = config_entry.get('name')
                is_array = count_children(config_entry, 'array') > 0
                if is_array:
                    assert_attribute_absent(config_entry, 'defaultValue', "when it's an array",
                                            locator)
                if 'isKey' in config_entry.attrib:
                    if not self.design_inspector.class_has_device_logic(class_name):
                        raise DesignFlaw(("isKey can only be used with device logic"
                                          "(at: {0})").format(stringify_locator(locator)))

    def validate_hasobjects(self, hasobjects, locator):
        """Performs validation of particular hasobjects element"""
        locator['hasobjects'] = hasobjects.get('class')
        locator['line_num'] = hasobjects.sourceline
        if hasobjects.get('instantiateUsing') == 'design':
            # stuff instantiated from design can't have any configuration-dependent things, just
            # purely address-space items
            inner_class = hasobjects.get('class')
            cls = self.design_inspector.objectify_class(inner_class)
            if count_children(cls, 'configentry') > 0:
                raise DesignFlaw(('For instantiation from design, only classes without config '
                                  'entries are allowed (at: {0})').format(
                                      stringify_locator(locator)))
            if count_children(cls, 'cachevariable') > 0:
                for ce in cls.cachevariable:
                    locator['inner_cachevariable'] = ce.get('name')
                    if ce.get('initializeWith') == 'configuration':
                        raise DesignFlaw(('For instantiation from design, only classes with non-'
                                          'config-dependent cachevariables are allowed (at: {0})')
                                         .format(stringify_locator(locator)))

        if hasobjects.get('instantiateUsing') == 'configuration':
            # as per OPCUA-1983, there should be no d:object entries in such instantiation./
            if count_children(hasobjects, 'object') > 0:
                raise DesignFlaw(("You can't have d:object when instantiation by configuration is "
                                  "chosen (at: {0})").format(stringify_locator(locator)))

    def validate_hasobjects_wrapper(self):
        """Performs validation of all hasobjects in the design, from all classes and root"""
        for class_name in self.design_inspector.get_names_of_all_classes():
            locator = {'class':class_name}
            for ho in self.design_inspector.objectify_has_objects(class_name):
                self.validate_hasobjects(ho, locator)
        root = self.design_inspector.objectify_root()
        if count_children(root, 'hasobjects'):
            locator = {'in':'root'}
            for ho in root.hasobjects:
                self.validate_hasobjects(ho, locator)

    def validate_methods(self):
        for class_name in self.design_inspector.get_names_of_all_classes():
            locator = {'class':class_name}
            cls = self.design_inspector.objectify_class(class_name)
            for m in self.design_inspector.objectify_methods(class_name):
                locator['method'] = m.get('name')
                if m.get('addressSpaceCallUseMutex') != 'no':
                    # here we deserve a couple of extra checks ...
                    if m.get('addressSpaceCallUseMutex') == 'of_containing_object':
                        self.assert_mutex_present(class_name, locator, 'to support '+m.get('addressSpaceCallUseMutex'))

def main():
    """It's just a helper main if you want to run this file stand-alone with pdb or so"""
    design_validator = DesignValidator('../Design/Design.xsd', '../Design/Design.xml')
    design_validator.validate()

if __name__ == '__main__':
    main()
