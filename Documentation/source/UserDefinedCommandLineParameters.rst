User Defined Command Line Parameters
====================================

Background
----------

An OPC-UA server may be a rather complex process, as such, developers
may well want to provide implementation specific command line options -
to allows end-users to control aspects of the server's runtime
behaviour, say specifying additional configuration options outside of
the quasar generated configuration file for example. This document
describes the means by which a developer can add their own command line
options to a server developed using quasar and retrieve the end-user
values for those options for treatment in their implementation code.

Boost Program Options
---------------------

quasar handles command line options using the Boost C++ library,
specifically the program_options library subset. To understand this
document, some knowledge of the program_options library is required:

-  Boost's full program options documentation can be found at
   `boost.org <https://www.boost.org>`__
-  The program options
   `tutorial <http://www.boost.org/doc/libs/1_60_0/doc/html/program_options/tutorial.html>`__
   could be a good place to start

The remainder of this document assumes sufficient knowledge of the
program options library.

Adding the command line options
-------------------------------

As is often the case in quasar, user specific implementation code is
included in the server by overriding methods. The virtual method to
override is ````

::

   BaseQuasarServer::appendCustomCommandLineOptions(boost::program_options::options_description& commandLineOptions, boost::program_options::positional_options_description& positionalOptionsDescription)

| During quasar server startup, on parsing the command line, overridden
  method 'appendCustomCommandLineOptions' will be called - this adds any
  implementation specific options to object 'commandLineOptions' and
  'positionalOptionsDescription' which is subsequently used to extract
  target values from the command line parameters supplied.
| Note that both parameters 'commandLineOptions' and
  'positionalOptionsDescription' are references to objects of class
  types defined in boost::program_options - for a full guide to the
  interface of this class see the boost documentation, what follows is
  just a simple example.

A Simple Example
----------------

This example shows how to add the 2 command line options listed below
such that a quasar OPC-UA server executable, once built, will parse for
these options, complete with end-user facing, option specifc,
documentation:

-  some_switch: A simple boolean switch (i.e. without any value;
   specifying the value at the command line implies TRUE, otherwise
   FALSE
-  some_string: A named string value

So, we want to be able to start the server process wit a command line
like: ````

::

   ./myServerExecutable [possibly other options here] --some_switch --some_string="hello, world!"

| Add variables to hold the specific command line values in C++ code,.
  In this case add them to class QuasarServer.
| Also add the virtual method override (also to class QuasarServer) Now
  the class definition pseudo code looks like...
| ````

::

   class QuasarServer : ...etc...
   {
       ...etc...
       virtual void appendCustomCommandLineOptions(boost::program_options::options_description& commandLineOptions, 
                                                       boost::program_options::positional_options_description& positionalOptionsDescription

); ... etc ... bool m_someSwitch; std::string m_someString; }

Now add the implementation of the method
'appendCustomCommandLineOptions', for example ````

::

   void QuasarServer::appendCustomCommandLineOptions(boost::program_options::options_description& commandLineOptions,                                                                           boost::program_options::positional_options_description& positionalOptionsDescription

) { commandLineOptions.add_options() ("some_switch",
boost::program_options::bool_switch(&m_someSwitch)->default_value(false),
"User documentation of some_switch") ("some_string",
boost::program_options::value(&m_someString)->default_value("anyDefaultValue"),
"User documentation of some_string"); }

Now, starting the server process with a command line above will result
in values ````

::

   QuasarServer::m_someSwitch  = true
   QuasarServer::m_someString "hello, world!"

Furthermore, starting the server process with a command line like ````

::

   ./myServerExecutable -help

will print the help documentation described above, at the command line,
for example: ````

::

   ...etc...
    --some_switch        User documentation of some_switch
    --some_string arg   User documentation of some_string
