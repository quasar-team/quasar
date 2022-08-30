User Defined Runtime Configuration Modification
===============================================

#. `User Defined Runtime Configuration Modification <#mozTocId42709>`__

   #. `Background <#mozTocId644599>`__
   #. `Building or Modifying the Configuration on Server Start Up
      (Configuration Decoration) <#mozTocId640162>`__

      #. `Key Functions <#mozTocId848996>`__
      #. `Example Code <#mozTocId626975>`__

         #. `Server Design <#mozTocId65675>`__
         #. `Function decorateConfiguration() <#mozTocId610798>`__
         #. `Function validateContentOrder <#mozTocId444708>`__

      #. `Wiring in the ConfigXmlDecoratorFunction
         instance <#mozTocId898308>`__

   #. `Persisting the Configuration in an XML file <#mozTocId11314>`__

Background
----------

| A completely static configuration for an OPC-UA server may not always
  be exactly the desired behaviour; for example - consider a
  multi-channel, highly modular, power supply system. For such a system
  it may be beneficial to users to provide functionality from the quasar
  server such that (with the correct command line options specified of
  course) the server actually detects the hardware that is connected and
  uses this as the basis for its runtime configuration. This might be an
  efficient way to run an OPC-UA server in situations where, say, the
  hardware setup is in flux (maybe an ad-hoc lab configuration) and so
  manual modification of a static configuration file to describe the
  system is painful. Equally, in a large production set up, the server
  could be run the first time in a 'discovery mode' to query the
  connected hardware (and persist the corresponding configuration in an
  XML file) and, assuming the generated configuration passes end-user
  review, i.e. correctly represents the hardware, used thereafter as the
  static configuration.
| The quasar framework provides a means to handle both runtime
  configuration modification (i.e. the 'discovery mode' mentioned above)
  and a means to persist configurations to XML files.
| It is assumed the reader already knows how to specify user specific
  command line options; described in document `User Defined Command Line
  Parameters <UserDefinedCommandLineParameters.html>`__
| Furthermore, knowledge of working with classes generated from schema
  documents is required, from the `code synthesis xsd
  library <http://www.codesynthesis.com/projects/xsd/>`__

Building or Modifying the Configuration on Server Start Up (Configuration Decoration)
-------------------------------------------------------------------------------------

| During server *build* time, quasar uses the Design.xml file to
  generate a configuration.xsd - a schema, determining the structure of
  all configuration instances (i.e configuration XML files, passed at
  the server command line via parameter *--config_file*). At build time
  quasar passes this configuration.XSD file to xsdcxx which generates a
  set of classes to handle configuration parsing (XML to an object tree
  of xsdcxx class instances) and serializing (an object tree of xsdcxx
  class instances to XML).
| During server *run* time, on initialisation, quasar parses the
  configuration.XML into a tree of xsdcxx objects. By default, this
  object tree is passed as input to the quasar Configurator code which
  creates and links Device and Address Space objects. However, quasar
  also provides a hook to allow extension of this configuration object
  tree before the Configurator is invoked. Using this hook, a server
  developer can extend the initial object tree with additional xsdcxx
  class objects, in a process called **Configuration Decoration**. The
  decorations are server specific: In the modular power supply example
  above, the tree is decorated with xsdcxx class instances according to
  the results of a system query - i.e. the modular hardware connected.

Key Functions
~~~~~~~~~~~~~

The start of of the configuration decoration hook is to override method
*overridableConfigure*. The virtual method is defined in class
*BaseQuasarServer*, and should be overridden in class *QuasarServer.
*

::

   virtual bool overridableConfigure(const std::string& fileName, AddressSpace::ASNodeManager *nm);

| The custom override is invoked before the object tree is processed by
  the Configurator logic. Server specific configuration decoration logic
  is added by providing an implementation of
  *ConfigXmlDecoratorFunction* (signature below) and passing that
  instance to an invocation of function *configure* (signature below).

::

   typedef std::function<bool (Configuration::Configuration&)> ConfigXmlDecoratorFunction;

   bool configure (std::string fileName,
           AddressSpace::ASNodeManager *nm, ConfigXmlDecoratorFunction
           configXmlDecoratorFunction = ConfigXmlDecoratorFunction()); // 'empty' function by default.

| The final required configuration decoration function is
  *Configuration::DecorationUtils::push_back* (file
  *ConfigurationDecorationUtils.h*). During execution of the
  *ConfigXmlDecoratorFunction* this function **must** be used to append
  new instances to the configuration object tree. The *push_back*
  function is described:

::

   /**
   * push_back is a configuration decoration helper function. This function *MUST*
   * be used when decorating the configuration object tree (i.e. adding new
   * object instances). This function handles both
   * 1. Addition: adding the object in the specified tree location.
   * 2. Content Ordering: maintaining the ordering mechanism quasar uses to process
   *    configuration elements in the correct order.
   *
   * @param parent: Parent tree node.
   * @param children: The collection of children (owned by the parent) to which the
   *        new child will be appended.
   * @param child: The new child object
   * @param childTypeId: The ordering type ID (as gen'd by xsdcxx) of the new child
   *        object
   */
   template< typename TParent, typename TChildren, typename TChild >
   void push_back(TParent& parent, TChildren& children, const TChild& child, const size_t childTypeId)

::

::

Example Code
~~~~~~~~~~~~

Server Design
^^^^^^^^^^^^^

| Server design is copied here to provide the composition structure.

::

   <?xml version="1.0" encoding="UTF-8"?>
   <d:design projectShortName="" xmlns:d="http://cern.ch/quasar/Design" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:schemaLocation="http://cern.ch/quasar/Design Design.xsd ">
     <d:class name="InnerClass">
       <d:devicelogic/>
       <d:configentry dataType="OpcUa_UInt32" name="configInstanceId"/>
       <d:cachevariable initializeWith="valueAndStatus" name="runtimeInstanceId" ...etc.../>
     </d:class>
     <d:class name="AnotherInnerClass">
       <d:devicelogic/>
       <d:configentry dataType="OpcUa_UInt32" name="configInstanceId"/>
       <d:cachevariable initializeWith="valueAndStatus" name="runtimeInstanceId" ...etc.../>
     </d:class>
     <d:class name="ConfiguredClass">
       <d:devicelogic/>
       <d:configentry dataType="OpcUa_UInt32" name="configInstanceId"/>
       <d:cachevariable initializeWith="valueAndStatus" name="runtimeInstanceId" ...etc.../>
       <d:hasobjects instantiateUsing="configuration" class="InnerClass"/>
       <d:hasobjects instantiateUsing="configuration" class="AnotherInnerClass"/>
     </d:class>
     <d:root>
       <d:hasobjects instantiateUsing="configuration" class="ConfiguredClass"/>
     </d:root>
   </d:design>
       

Function decorateConfiguration()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| The configuration decoration logic is added to class QuasarServer in
  function *decorateConfiguration*. See comments in snippet below
  describing the goal of the code.

::

   bool QuasarServer::decorateConfiguration(Configuration::Configuration& theConfig) const
   {
     LOG(Log::INF) <<__FUNCTION__<< " starting server specific configuration decoration";
     // Goal: extend given configuration (theConfig) AT RUNTIME (initialisation) as follows
     //
     // theConfig (contains contents of config.xml)
     // +
     // |_ConfiguredClass(1000)
     //   |_InnerClass(1001)
     //   |_AnotherInnerClass(1002)
     //   |_InnerClass(1003)
     //   |_AnotherInnerClass(1004) 
     // |_ConfiguredClass(2000)
     //    |_InnerClass(2001)
     //    |_AnotherInnerClass(2002)
     //    |_InnerClass(2003)
     //    |_AnotherInnerClass(2004)
     // |_ConfiguredClass(3000)
     //    |_InnerClass(3001)
     //    |_AnotherInnerClass(3002)
     //    |_InnerClass(3003)
     //    |_AnotherInnerClass(3004)

     // Create & populate objects locally, then add to theConfig tree using quasar decoration utility function
     for(int i=1000; i<=3000; i+=1000)
     {
       Configuration::ConfiguredClass parent("parentDevice"+std::to_string(i), i);

       for(int j = i+1; j<=i+4; ++j)
       {
         if(j%2)
         {
           Configuration::InnerClass child("childDevice"+std::to_string(j), j);
           Configuration::DecorationUtils::push_back(parent, parent.InnerClass(), child,
                                                     Configuration::ConfiguredClass::InnerClass_id);
         }
         else
         {
           Configuration::AnotherInnerClass child("anotherChildDevice"+std::to_string(j), j);
           Configuration::DecorationUtils::push_back(parent, parent.AnotherInnerClass(), child,
                                                     Configuration::ConfiguredClass::AnotherInnerClass_id);
         }
       }
       Configuration::DecorationUtils::push_back(theConfig, theConfig.ConfiguredClass(), parent,
                                                 Configuration::Configuration::ConfiguredClass_id);
     }
     LOG(Log::INF) <<__FUNCTION__<< " completed server specific configuration decoration";
     return true;
   }

| The object instatiation above (**in blue**) are direct calls to xsdcxx
  generated code: i.e. xsdcxx class constructors. Note that the actual
  constructors depend on the server design (these classes are generated
  from the configuration XSD which is generated from Design XML).
| The object addition above (**in dark green**) are calls to quasar's
  helper function, required to append new objects to the configuration.
| What if removal of elements is required? Trivial removal (e.g.
  invoking clear() on children sequence) would of course break the
  content order data. For some applications (e.g. clear()) we provide
  functions in the DecorationUtils, e.g.

.. container::

   .. container::

      Configuration::DecorationUtils::clear(theConfig,
      theConfig.ConfiguredClass(),
      Configuration::Configuration::ConfiguredClass_id);

| 

Function validateContentOrder
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section is here for information: There is no action required by
server developers. Internally, quasar calls *validateContentOrder*
during initialisation to verify the configuration object tree is valid
with respect to the content order mechanism, including any additional
objects that were added to the tree during configuration decoration.
Below is some deliberately erroneous code and the corresponding error
message quasar logs as it exist (due to *validateContentOrder* failing).

| Erroneous code: Does *not* call required function
  **Configuration::DecorationUtils::push_back** to add an object to the
  tree during decoration. This breaks the xsdcxx content order
  mechanism.

::

         if(j%2)
         {
           Configuration::InnerClass child("childDevice"+std::to_string(j), j);
       parent.InnerClass().push_back(child); // WRONG ! INVALIDATES CONTENT ORDER
         }

| Code such as this results in quasar terminating on startup - the
  server throws an exception with a message.

::

   2020-05-19 17:37.04.106695 [BaseQuasarServer.cpp:156, ERR] Exception caught in BaseQuasarServer::serverRun:  [validateContentOrder ERROR parent has [2] child objects unregistered in content order]

Wiring in the ConfigXmlDecoratorFunction instance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The final step is to ensure that the function ``configure`` above is
called with the correct arguments; namely with the developer's
implementation of ConfigXmlDecoratorFunction as the 3rd argument. As is
often the case in quasar, injecting user specifc code involves
overriding a virtual function. In this case, the virtual function to
override is: ````

::

   bool BaseQuasarServer::overridableConfigure(const std::string& fileName, AddressSpace::ASNodeManager *nm);

A typical developer override of this function would be along the lines
of the following pseudo code ````

::

   bool QuasarServer::overridableConfigure(const std::string& fileName, AddressSpace::ASNodeManager *nm)
   {
       if([command line switch active for discovery mode])
       {
           LOG(Log::INF) <<__FUNCTION__<< " server specific override invoked, configuration will be decorated";
           ConfigXmlDecoratorFunction decoratorFn = std::bind(&QuasarServer::decorateConfiguration, this, std::placeholders::_1);
           return configure(fileName, nm, decoratorFn);
       }
       else
       {
           LOG(Log::INF) <<__FUNCTION__<< " server running in regular mode, configuration will be as per config.xml";
           return configure(fileName, nm);
       }
   }

.. _persist:

Persisting the Configuration in an XML file
-------------------------------------------

As described above, type definition ``ConfigXmlDecoratorFunction``
describes a single parameter ``Configuration::Configuration&``, this
parameter is an instance of an XSD generated C++ class which handles
both in-memory object loading from an XML file (parsing/deserialization)
and, the key point here, writing the contents of the in-memory object to
an XML file (serialization). To persist an in-memory configuration then,
we need simply only call serialization methods from xsd-cxx. Once
in-memory configuration decoration is complete; it can be written to an
XML file by calling (something like) the following pseudo-code excerpt:

::

   std::ofstream configurationFile;

   try
   {
     configurationFile.open(some command line specified path for the config serialization, ofstream::out | ofstream::trunc);
   }
   catch(...all sorts of errors....)
   { ...and handle... }

   try
   {
     xml_schema::namespace_infomap nsMap;
     nsMap[""].name = "http://cern.ch/quasar/Configuration";
     nsMap[""].schema = "../Configuration/Configuration.xsd";

     Configuration::configuration(configurationFile, theConfiguration, nsMap); // actual write executed on this line
   }
   catch(...all sorts of errors....)
   { ...and handle... }
       
