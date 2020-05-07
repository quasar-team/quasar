#!/usr/bin/env python
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

def stringify_locator(locator):
    """Creates comma-separated representation of the dictionary in another font color. It is to
       point out where (in which class, variable, etc) given problem appeared"""
    locator_as_list = ["{0}={1}".format(key, locator[key]) for key in locator]
    return Fore.MAGENTA + ','.join(locator_as_list) + Style.RESET_ALL

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
    except Exception:
        pass  # that's OK, it's simply not there
    return count

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
        self.validate_config_entries()

    def validate_initial_value(self, cachevariable):
        """initialValue is there, but its format depends on dataType so can't be validated by XSD"""
        # TODO : this is per OPCUA-1778
        pass

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

    def validate_cache_variables(self):
        """Performs validation of all cache variables in the design"""
        for class_name in self.design_inspector.get_names_of_all_classes():
            locator = {'class':class_name}
            cls = self.design_inspector.objectify_class(class_name)
            if count_children(cls, 'cachevariable') == 0:
                continue
            for cache_variable in cls.cachevariable:
                locator['cachevariable'] = cache_variable.get('name')
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
                        self.validate_initial_value(cache_variable)
                if count_children(cache_variable, 'array') > 0:
                    self.validate_array(cache_variable.array, locator)
                if cache_variable.get('dataType') == 'UaVariant':
                    assert_attribute_equal(cache_variable, 'initializeWith', 'valueAndStatus',
                                           'when data type is UaVariant', locator)
                    assert_attribute_absent(cache_variable, 'initialValue',
                                            'when data type is UaVariant', locator)
                if 'isKey' in cache_variable.attrib:
                    if not self.design_inspector.class_has_device_logic(class_name):
                        raise DesignFlaw(("isKey can only be used with device logic"
                                          "(at: {0})").format(stringify_locator(locator)))
                    if cache_variable.array is not None:  # TODO wrong
                        raise DesignFlaw('isKey can not be used with arrays (at: {0})'.format(
                            stringify_locator(locator)))

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

def main():
    """It's just a helper main if you want to run this file stand-alone with pdb or so"""
    design_validator = DesignValidator('../Design/Design.xsd', '../Design/Design.xml')
    design_validator.validate()

if __name__ == '__main__':
    main()
