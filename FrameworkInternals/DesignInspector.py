#!/usr/bin/env python3
# encoding: utf-8
'''
DesignInspector.py

@author:     Piotr Nikiel <piotr@nikiel.info>
@author:     Ben Farnham  <firstNm.secondNm@cern.ch>

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
'''

from lxml import etree, objectify
from colorama import Fore, Style
from lxml.objectify import ObjectifiedDataElement
from copy import deepcopy
import logging

DEBUG = False
DEBUG_XPATH = False

QUASAR_NAMESPACES = {'d':'http://cern.ch/quasar/Design'}

class DesignInspector():
    """This class is to dig out data of interest for quasar NextGen transforms"""
    def __init__(self, designPath):
        design_file = open(designPath, 'r', encoding='utf-8')
        self.tree = etree.parse(design_file)
        # TODO: get root here if it is guaranteed to exist?

    def xpath(self, expr, *args):
        """ Just a wrapper on top of etree.xpath that does quasar namespaces mapping """
        xpath_expr = expr.format(*args)
        result = self.tree.xpath(xpath_expr, namespaces=QUASAR_NAMESPACES)
        if DEBUG_XPATH:
            print(Fore.YELLOW + "xpath({0}) gives {1}".format(
                xpath_expr, result)
                  + Style.RESET_ALL)
        return result

    def xpath_relative_to_object(self, object, xpath_expression):
        """ Executes xpath query relative to 'object'. Expectation is that
        object is of type ObjectifiedElement, or at least has method xpath
        matching the signature of ObjectifiedElement.xpath()"""
        return object.xpath(xpath_expression, namespaces=QUASAR_NAMESPACES)

    def get_names_of_all_classes(self, only_with_device_logic=False):
        """Returns a list of names of all classes from the design.
        Unless only_with_device_logic is True, returns classes with and without
        device logic"""
        xpath_expression = 'd:class'
        if only_with_device_logic:
            xpath_expression += '[d:devicelogic]'
        classes = self.xpath(xpath_expression)
        classes_names = [klass.attrib['name'] for klass in classes]
        logging.debug(f'Names of all classes: {classes_names}')
        return classes_names

    def class_has_device_logic(self, class_name):
        """ Returns true if given class has device logic """
        # TODO: should throw an exception when no such class
        device_logic = self.xpath('/d:design/d:class[@name="{0}"]/d:devicelogic'.format(class_name))
        return len(device_logic) > 0 ## no devicelogic element means empty list returned

    def get_class_default_instance_name(self, class_name):
        """Returns default instance name or None if not applicable"""
        default_instance_name = self.xpath(
            f'/d:design/d:class[@name="{class_name}"]/@defaultInstanceName')
        return None if len(default_instance_name) == 0 else default_instance_name[0]

    def get_has_objects_origin_names(self, class_name, include_root=False):
        """Finds all classes (and Root, if requested) that have has_objects
        pointing to class_name """
        classes = self.xpath(
            "/d:design/d:class[d:hasobjects/@class='{0}']".format(class_name))
        classes = [x.attrib['name'] for x in classes]
        if include_root:
            root = self.xpath(
                "/d:design/d:root[d:hasobjects/@class='{0}']".format(class_name))
            if len(root) == 1: # will be an empty list if none
                classes += ['Root']
        return classes

    def class_has_legit_device_parent(self, class_name):
        """Returns True if there exists a particular C++ Device Logic type that is a parent to
        given class"""
        if not self.class_has_device_logic(class_name):
            return False
        has_objects_origins = self.get_has_objects_origin_names(
            class_name, include_root=True)
        if len(has_objects_origins) != 1:
            return False # referenced from multiple parents, so far sure no unique parent
        if has_objects_origins[0] == "Root":
            return True # Root by definition has device logic
        if not self.class_has_device_logic(has_objects_origins[0]):
            return False
        return True

    def get_parent(self, class_name):
        """Returns parent class name or None if device-logic parent is not applicable"""
        if self.class_has_device_logic(class_name):
            has_objects_origins = self.get_has_objects_origin_names(class_name, include_root=True)
            if len(has_objects_origins) == 1:
                return has_objects_origins[0]
            else:
                return None  # many parents, so no single parent.
        else:
            return None  # child class w/o DeviceLogic, so no device lo

    def get_class_has_objects(self, class_name):
        """Returns a list of names of all classes that are 'children'
        (in has_objects) sense of given class."""
        # TODO: should throw a NoSuchClass exception when no class!
        has_objects = self.xpath("/d:design/d:class[@name='{0}']/d:hasobjects".format(class_name))
        return has_objects

    def is_has_objects_singleton_any(self, has_objects):
        """Returns True if given hasObjects is a singleton. A version for lxml.etree"""
        # TODO @pnikiel merge both versions
        return ("minOccurs" in has_objects.attrib
                and has_objects.attrib['minOccurs'] == "1"
                and "maxOccurs" in has_objects.attrib
                and has_objects.attrib['maxOccurs'] == "1")

    def is_has_objects_singleton_any2(self, has_objects):
        """Returns True if given hasObjects is a singleton. A version for lxml.objectify"""
        # TODO @pnikiel merge both versions
        return ("minOccurs" in has_objects.keys()
                and "maxOccurs" in has_objects.keys()
                and has_objects.get("minOccurs") == "1"
                and has_objects.get("maxOccurs") == "1")
                
    def is_class_always_singleton(self, class_name, instantiated_by_filter=None):
        """Returns True if every instance of the target class is a singleton, with
           instantiation mechanism (design/configuration) optionally filtered"""
        filter_str = '' if instantiated_by_filter is None else f"and @instantiateUsing='{instantiated_by_filter}'"
        has_objects_list = self.objectify_any(f"//d:hasobjects[@class='{class_name}' {filter_str}]")
        if len(has_objects_list) > 0:
            is_singleton = lambda has_objects: self.is_has_objects_singleton_any2(has_objects)
            return all(map(is_singleton, has_objects_list))
        return False

    def has_objects_class_names(self, class_name, only_with_device_logic=False):
        """Returns a list of names of all classes that are 'children' (in has_objects) sense of
        given class"""
        # TODO: should throw a NoSuchClass exception for no class
        has_objects = self.xpath("/d:design/d:class[@name='{0}']/d:hasobjects".format(class_name))
        has_objects_class_names = [has_object.attrib['class'] for has_object in has_objects]
        if only_with_device_logic:
            has_objects_class_names = [x for x in has_objects_class_names
                                       if self.class_has_device_logic(x)]
        return has_objects_class_names

    def device_logic_has_mutex(self, class_name):
        """Returns True if class 'class_name' device logic has mutex"""
        mutex = self.xpath(
            "/d:design/d:class[@name='{0}']/d:devicelogic/d:mutex".format(class_name))
        return len(mutex) > 0  ## no mutex element means empty list returned

    def objectify_class(self, class_name):
        """Returns lxml.objectify of given class"""
        class_element = self.xpath("/d:design/d:class[@name='{0}']".format(class_name))
        if len(class_element) != 1:
            raise Exception(
                "ERROR: Class {0} NOT FOUND in your quasar design... ".format(class_name))
        objectified = objectify.fromstring(etree.tostring(class_element[0]))
        return objectified

    def objectify_root(self):
        """Returns lxml.objectify of d:root"""
        root_element = self.xpath("/d:design/d:root")
        return objectify.fromstring(etree.tostring(root_element[0]))

    def objectify_any(self, xpath_expression, *args):
        """Returns lxml.objectify of anything, this basically is a nice wrapper."""
        results = self.xpath(xpath_expression.format(*args))
        objectified = [objectify.fromstring(etree.tostring(x)) for x in results]
        return objectified

    def objectify_has_objects(self, class_name, restrict_by=''):
        """Returns list of lxml.objectify of hasObjects"""
        return self.objectify_any(
            "/d:design/d:class[@name='{0}']/d:hasobjects{1}".format(
                class_name, restrict_by))

    def objectify_cache_variables(self, class_name, restrict_by=''):
        """Returns a list of lxml.objectify of cache-vars of given class"""
        return self.objectify_any(
            "/d:design/d:class[@name='{0}']/d:cachevariable{1}".format(
                class_name, restrict_by))

    def objectify_config_entries(self, class_name, restrict_by=''):
        """Returns a list of lxml.objectify of config-entries of given class"""
        return self.objectify_any(
            "/d:design/d:class[@name='{0}']/d:configentry{1}".
            format(class_name, restrict_by))

    def objectify_source_variables(self, class_name, restrict_by=''):
        """Returns a list of lxml.objectify of source-vars of given class"""
        return self.objectify_any(
            "/d:design/d:class[@name='{0}']/d:sourcevariable{1}".format(class_name, restrict_by))

    def objectify_methods(self, class_name, restrict_by=''):
        """Returns a list of lxml.objectify of methods of given class"""
        return self.objectify_any(
            "/d:design/d:class[@name='{0}']/d:method{1}".format(class_name, restrict_by))

    def objectify_design(self):
        """Returns lxml.objectify of d:design"""
        return self.objectify_any("/d:design")[0]

    def design_boolean_as_cpp_boolean(self, attribute):
        """Transforms a value from Design to a C++ boolean"""
        if attribute is None:
            return 'false'
        if attribute:
            return 'true'
        return 'false'

    def is_class_single_variable_node(self, class_name):
        """Returns True if the class is a singleVariableNode"""
        the_class = self.objectify_class(class_name)
        return the_class.get('singleVariableNode') == 'true'

    def get_restrictions(self, class_name, name, what):
        """Returns a list of tuples(type, value) where type is one of [enumeration, pattern,
           minExclusive, minInclusive, maxInclusive, maxExclusive] and value is plug'n'play value
           attribute of chosen XSD restriction.

           what - either 'cachevariable' or 'configentry'"""

        list_of_tuples = []

        xpath_prefix = "/d:design/d:class[@name='{0}']/d:{1}[@name='{2}']/d:configRestriction".format(class_name, what, name)
        for enumerationValue in self.objectify_any(xpath_prefix + "/d:restrictionByEnumeration/d:enumerationValue"):
            list_of_tuples.append(('enumeration', enumerationValue.get('value'), ))
        for restrictionByPattern in self.objectify_any(xpath_prefix + "/d:restrictionByPattern"):
            list_of_tuples.append(('pattern', restrictionByPattern.get('pattern'), ))
        for restrictionByBounds in self.objectify_any(xpath_prefix + "/d:restrictionByBounds"):
            list_of_tuples.extend([(key, restrictionByBounds.attrib[key]) for key in restrictionByBounds.attrib.keys()])

        return list_of_tuples

    def getProjectName(self):
        return self.xpath("/d:design/@projectShortName")[0]

    def objectifyAllParents(self, className, restrict_to_by_configuration=False):
        #pdb.set_trace()
        xpath_string="//d:hasobjects[@class='{0}'".format(className)
        if(restrict_to_by_configuration):
            xpath_string+=" and @instantiateUsing='configuration'"
        xpath_string+="]/.."
        return self.xpath(xpath_string)

    def objectifyDocumentation(self, className, cachevar_or_configentry_name=''):
        """ if cachevar_or_config_entry_name empty, returns class documentation element as object """
        """ otherwise returns specified cachevar/configentry documentation element as object """
        if not cachevar_or_configentry_name:
            return self.objectify_any("/d:design/d:class[@name='{0}']/d:documentation".format(className))
        else:
            xpath_str="\
            /d:design/d:class[@name='{0}']/d:cachevariable[@name='{1}']/d:documentation \
            | \
            /d:design/d:class[@name='{0}']/d:configentry[@name='{1}']/d:documentation".format(className, cachevar_or_configentry_name)
            return self.objectify_any(xpath_str)

    def strip_documentation_for_xsd(self, documentation_object):
        """ documentation_object is as returned from objectify_documentation; an objectified
        documentation element. Required since documentation may require markup tags (e.g HTML)
        which are not permitted in the configuration XSD. Use this function to strip those tags"""
        mutable_documentation_object = deepcopy(documentation_object)
        etree.strip_tags(mutable_documentation_object, '*')
        return mutable_documentation_object.text

    def strip_documentation_tag(self, documentation_element):
        """Returns the stringified meat from XML documentation element <d:documentation...>"""
        stringified = etree.tostring(documentation_element).decode('utf-8')
        opening_tag_ends = stringified.find('>')
        closing_tag_starts = stringified.rfind('<')
        meat = stringified[opening_tag_ends+1 : closing_tag_starts]
        return meat

    def to_list_if_exists(self, iterable):
        try:
            return list(iterable)
        except ValueError:
            return []

if __name__ == "__main__":
    """Mostly as test fixture"""
    di = DesignInspector('../Design/Design.xml')
