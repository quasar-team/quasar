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
        
    def get_restriction(self, className, cachevar_or_configentry_name):
        xpath_str="\
            /d:design/d:class[@name='{0}']/d:cachevariable[@name='{1}']/d:configRestriction \
            | \
            /d:design/d:class[@name='{0}']/d:configentry[@name='{1}']/d:configRestriction".format(className, cachevar_or_configentry_name)
        restrictionElems=self.objectify_any(xpath_str)
        # cache_variables = self.objectify_cache_variables(className, "[@name='{0}' and d:configRestriction]".format(cachevar_or_configentry_name))
        # config_entries = self.objectify_config_entries(className, "[@name='{0}' and d:configRestriction]".format(cachevar_or_configentry_name))
        # config_restrictions = [element.configRestriction for element in cache_variables + config_entries]
        return self.parseRestrictions(restrictionElems)
        
    def parseRestrictions(self, restrictionElems):
        """ Method returns a dict populated according to the (objectified) restrictions"""
        """ passed. Note that the assumption is there is *at most one* restriction permitted"""
        """ in the parent cachevar/configentry XML excerpt """ 
        if(len(restrictionElems)==1):
            restrictionElem = restrictionElems[0]

            restriction=dict()
            restriction['type']='ERROR'
            for child in restrictionElem.iterchildren():
                if 'restrictionByEnumeration' in child.tag:
                    restriction['type']='byEnumeration'
                    restriction['enumerationValues']=[]
                    for enumerationValue in child.iterchildren():
                        restriction['enumerationValues'].append(enumerationValue.get('value'))
                    break
                elif 'restrictionByPattern' in child.tag:
                    restriction['type']='byPattern'
                    restriction['pattern']=child.get('pattern')
                    break;
                elif 'restrictionByBounds' in child.tag:
                    restriction['type']='byBounds'
                    restriction['minInclusive']=child.get('minInclusive')
                    restriction['maxInclusive']=child.get('maxInclusive')
                    restriction['minExclusive']=child.get('minExclusive')
                    restriction['maxExclusive']=child.get('maxExclusive')
                    break;
            return restriction
        return None
        
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
