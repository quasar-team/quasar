#!/usr/bin/env python
# encoding: utf-8
'''
DesignInspector.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

@license:
Copyright (c) 2015, CERN.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS  OR IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED
WARRANTIES  OF  MERCHANTABILITY  AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL DAMAGES
(INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR  SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON
ANY  THEORY  OF  LIABILITY,   WHETHER IN  CONTRACT, STRICT  LIABILITY,  OR  TORT
(INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT OF  THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

from lxml import etree, objectify
from colorama import Fore, Style

DEBUG = False
DEBUG_XPATH = False

QUASAR_NAMESPACES = {'d':'http://cern.ch/quasar/Design'}

class DesignInspector():
    """This class is to dig out data of interest for quasar NextGen transforms"""
    def __init__(self, designPath):
        design_file = open(designPath, 'r')
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

    def get_names_of_all_classes(self, only_with_device_logic=False):
        """Returns a list of names of all classes from the design.
        Unless only_with_device_logic is True, returns classes with and without
        device logic"""
        xpath_expression = 'd:class'
        if only_with_device_logic:
            xpath_expression += '[d:devicelogic]'
        classes = self.xpath(xpath_expression)
        classes_names = map(lambda x: x.attrib['name'], classes)
        return classes_names

    def class_has_device_logic(self, class_name):
        """ Returns true if given class has device logic """
        # TODO: should throw an exception when no such class
        device_logic = self.xpath('/d:design/d:class[@name="{0}"]/d:devicelogic'.format(class_name))
        return len(device_logic) > 0 ## no devicelogic element means empty list returned

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

    def classHasLegitDeviceLogicParent(self, class_name):
        if not self.class_has_device_logic(class_name):
            return False
        has_objects_origins = self.get_has_objects_origin_names(
            class_name, include_root=True)
        if len(has_objects_origins) != 1:
            return False
        if has_objects_origins[0] == "Root":
            return True
        if not self.class_has_device_logic(has_objects_origins[0]):
            return False
        return True

    def get_parent_struct(self, class_name):
        """ Returns whatever should be typedef'ed as Parent_DX for X = class_name """
        # a case where device logic parent cant be established
        no_parent_structure = 'struct{/*No exact Parent of the class*/}'
        if self.class_has_device_logic(class_name):
            has_objects_origins = self.get_has_objects_origin_names(class_name, include_root=True)
            if len(has_objects_origins) == 1:
                parent_structure = 'D'+has_objects_origins[0]
            else:
                parent_structure = no_parent_structure
        else:
            parent_structure = no_parent_structure
        if DEBUG:
            print(Fore.YELLOW
                  + "get_parent_struct({0}) gives {1}".format(
                      class_name, parent_structure)
                  + Style.RESET_ALL)
        return parent_structure

    def get_class_has_objects(self, class_name):
        """Returns a list of names of all classes that are 'children'
        (in has_objects) sense of given class."""
        # TODO: should throw a NoSuchClass exception when no class!
        has_objects = self.xpath("/d:design/d:class[@name='{0}']/d:hasobjects".format(class_name))
        return has_objects

    def is_has_objects_singleton_any(self, has_objects):
        return ("minOccurs" in has_objects.attrib
                and has_objects.attrib['minOccurs'] == "1"
                and "maxOccurs" in has_objects.attrib
                and has_objects.attrib['maxOccurs'] == "1")

    def is_has_objects_singleton_any2(self, has_objects):
        """ TODO merge this with one, this should favour the objectified version """
        return ("minOccurs" in has_objects.keys()
                and "maxOccurs" in has_objects.keys()
                and has_objects.get("minOccurs") == "1"
                and has_objects.get("maxOccurs") == "1")

    def get_class_has_objectsClassNames(self, class_name, only_with_device_logic=False):
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
        class_element = self.xpath("/d:design/d:class[@name='{0}']".format(class_name))
        if len(class_element) != 1:
            raise Exception(
                "ERROR: Class {0} NOT FOUND in your quasar design... ".format(class_name))
        objectified = objectify.fromstring(etree.tostring(class_element[0]))
        return objectified

    def objectify_root(self):
        root_element = self.xpath("/d:design/d:root")
        return objectify.fromstring(etree.tostring(root_element[0]))

    def objectify_any(self, xpath_expression, *args):
        results = self.xpath(xpath_expression.format(*args))
        objectified = [objectify.fromstring(etree.tostring(x)) for x in results]
        return objectified

    def objectify_has_objects(self, class_name, restrict_by=''):
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
        return self.objectify_any("/d:design")[0]

    def design_boolean_as_cpp_boolean(self, attribute):
        if attribute is None:
            return 'false'
        if attribute:
            return 'true'
        return 'false'

    def is_class_single_variable_node(self, class_name):
        the_class = self.objectify_class(class_name)
        return the_class.get('singleVariableNode') == 'true'
