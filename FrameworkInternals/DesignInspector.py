# TODO header

from lxml import etree, objectify
import pdb
from colorama import Fore, Style
DEBUG = False
DEBUG_XPATH = False

quasar_namespaces = {'d':'http://cern.ch/quasar/Design'}

class DesignInspector():
    def __init__(self, designPath):
        f = open(designPath,'r')
        self.tree = etree.parse(f)
        # TODO: get root here if it is guaranteed to exist?

    def xpath(self, expr, *args):
        """ Just a wrapper on top of etree.xpath that does quasar namespaces mapping """
        xpath_expr = expr.format(*args)
        result = self.tree.xpath(xpath_expr, namespaces = quasar_namespaces)
        if DEBUG_XPATH:
            print(Fore.YELLOW + "xpath({0}) gives {1}".format(
                xpath_expr, result)
                 + Style.RESET_ALL)
        return result

    def getNamesOfAllClasses(self, only_with_device_logic=False):
        """ Returns a list of names of all classes from the design.
            Unless only_with_device_logic is True, returns classes with and without device logic. """
        xpath_expression = 'd:class'
        if only_with_device_logic:
            xpath_expression += '[d:devicelogic]'
        classes = self.xpath(xpath_expression)
        classes_names = map(lambda x: x.attrib['name'], classes)
        return classes_names
    
    def getParentClassName(self, className):
        ## TODO: deprecated, remove it
        return 'Parent_D'+className

    def classHasDeviceLogic(self, className):
        """ Returns true if given class has device logic """
        """ TODO: should throw an exception when no such class """
        device_logic = self.xpath('/d:design/d:class[@name="{0}"]/d:devicelogic'.format(className))
        return len(device_logic) > 0 ## no devicelogic element means empty list returned

    def getHasObjectsOriginNames(self, className, include_root=False):
        """ TODO: is broken !! """
        """ Finds all classes (and Root, if requested) that have hasObjects pointing to className """
        classes = self.xpath("/d:design/d:class[d:hasobjects/@class='{0}']".format(className))
        classes = [x.attrib['name'] for x in classes]
        if include_root:
            root = self.xpath("/d:design/d:root[d:hasobjects/@class='{0}']".format(className))
            if len(root) == 1: # will be an empty list if none
                classes += ['Root']
        if DEBUG:
            print(Fore.YELLOW + "getHasObjectsOriginNames({0}, include_root={1}) gives {2}".format(
                className, include_root, classes)
                 + Style.RESET_ALL)
        return classes

    def classHasLegitDeviceLogicParent(self, className):
        if not self.classHasDeviceLogic(className):
            return False
        hasObjects_origins = self.getHasObjectsOriginNames(className, include_root=True)
        if len(hasObjects_origins) != 1:
            return False
        if hasObjects_origins[0] == "Root":
            return True
        if not self.classHasDeviceLogic(hasObjects_origins[0]):
            return False
        return True

    def getParentStruct(self, className):
        """ Returns whatever should be typedef'ed as Parent_DX for X = className """
        "TODO: rename to getDeviceLogicParentStruct "
        "TODO: ensure that both parent and child have device logic"
        NoParentStructure = 'struct{/*No exact Parent of the class*/}' # a case where device logic parent cant be established 
        if self.classHasDeviceLogic(className):
            hasObjects_origins = self.getHasObjectsOriginNames(className, include_root=True)
            if len(hasObjects_origins) == 1:
                parentStructure = 'D'+hasObjects_origins[0]
            else:
                parentStructure = NoParentStructure
        else:
            parentStructure = NoParentStructure
        if DEBUG:
            print(Fore.YELLOW + "getParentStruct({0}) gives {1}".format(className, parentStructure) + Style.RESET_ALL)
        return parentStructure

    def getClassHasObjects(self, className, only_with_device_logic=False):
        """ Returns a list of names of all classes that are 'children' (in hasObjects) sense of given class """
        """ TODO: should throw a NoSuchClass exception for no class """
        has_objects = self.xpath("/d:design/d:class[@name='{0}']/d:hasobjects".format(className))
        return has_objects

    def isHasObjectsSingleton(self, hasObjects):
        return "minOccurs" in hasObjects.attrib and hasObjects.attrib['minOccurs'] == "1" and "maxOccurs" in hasObjects.attrib and hasObjects.attrib['maxOccurs'] == "1"
    
    def isHasObjectsSingleton2(self, hasObjects):
        """ TODO merge this with one, this should favour the objectified version """
        return "minOccurs" in hasObjects.keys() and "maxOccurs" in hasObjects.keys() and hasObjects.get("minOccurs") == "1" and hasObjects.get("maxOccurs") == "1"


    def getClassHasObjectsClassNames(self, className, only_with_device_logic=False):
        """ Returns a list of names of all classes that are 'children' (in hasObjects) sense of given class """
        """ TODO: should throw a NoSuchClass exception for no class """
        has_objects = self.xpath("/d:design/d:class[@name='{0}']/d:hasobjects".format(className))
        has_objects_class_names = [ has_object.attrib['class'] for has_object in has_objects ]
        if only_with_device_logic:
            has_objects_class_names = [ x for x in has_objects_class_names if self.classHasDeviceLogic(x) ]
        if DEBUG:
            print(Fore.YELLOW + "getClassHasObjectsClassNames({0}, only_with_device_logic={2}) gives {1}".format(className, has_objects_class_names, only_with_device_logic) + Style.RESET_ALL)
        return has_objects_class_names

    def classDeviceLogicHasMutex(self, className):
        mutex = self.xpath("/d:design/d:class[@name='{0}']/d:devicelogic/d:mutex".format(className))
        return len(mutex) > 0  ## no mutex element means empty list returned

    def objectifyClass(self, className):
        class_element = self.xpath("/d:design/d:class[@name='{0}']".format(className))
        if len(class_element) != 1:
            raise Exception ("TODO!!")
        objectified = objectify.fromstring( etree.tostring(class_element[0]) )
        return objectified

    def objectifyRoot(self):
        root_element = self.xpath("/d:design/d:root")
        return objectify.fromstring( etree.tostring(root_element[0]) )

    def objectifyAny(self, xpathExpression, *args):
        results = self.xpath(xpathExpression.format(*args))
        objectified = [ objectify.fromstring( etree.tostring(x)) for x in results ]
        return objectified

    def objectifyCacheVariables(self, className, restrictBy=''):
        return self.objectifyAny("/d:design/d:class[@name='{0}']/d:cachevariable{1}".format(className, restrictBy))
        
    def objectifyConfigEntries(self, className, restrictBy=''):
        return self.objectifyAny("/d:design/d:class[@name='{0}']/d:configentry{1}".format(className, restrictBy))

