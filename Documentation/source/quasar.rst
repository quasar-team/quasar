quasar
======

The quick opcua server generation framework.

For quick start-up, see our `YouTube
  tutorial <https://www.youtube.com/watch?v=v212aPmbahM&list=PLz6bxFrT1-KBZxoSxr4ZvlTyxNeYE3L7b>`__.

| Primary contact: quasar-developers@cern.ch

Versions
--------

`Changelog <ChangeLog.html>`__

Credits
-------

This framework was brought to you by (in the alphabetical order):

- Damian Abalo Miron (damian.abalo@cern.ch)
   -  Windows compatibility and MSVC10 support
   -  Python-based scripts and tools

- Ingo Berg (external contributor not affiliated with CERN)
   -  muParser formula parser and evaluation engine, used by the
      CalculatedVariables and shipped with quasar. (`muParser's
      webpage <https://beltoforion.de/en/muparser/>`__)

- Benjamin Farnham (benjamin.farnham@cern.ch):
   -  LogIt logging engine
   -  Meta module
   -  Use cases & ideas

- Viatcheslav "Slava" Filimonov (viatcheslav.filimonov@cern.ch):
   -  XML Configuration (inspiration)
   -  Calculation engine (inspiration)
   -  Node manager

- Paris Moschovakos (paris.moschovakos@cern.ch):
   -  quasar lead from Sep 2022
   -  mule (SNMP optional module)
   -  UaObjects
   -  documentation revamp

- Piotr Nikiel (piotr.nikiel@cern.ch, piotr@nikiel.info):
   -  Framework's concept and architecture and code-generation
   -  Address Space, Device and Configuration generation (XSLTs, design
      files, scripts, Jinja2-based rewrite)
   -  CMake-based build system
   -  RPM generation for Linux
   -  Documentation and screencasts
   -  Build configurations for Raspberry Pi, Zynq and MSVC10 on Wine
   -  Yocto and PetaLinux support
   -  CalculatedVariables, Cacophony, UaObjects...
   -  quasar lead from its ground zero (2014) until Sep 2022

- Stefan Schlenker (stefan.schlenker@cern.ch)
   -  Use cases, ideas, optional modules mechanism

- Cristian-Valeriu Soare
   -  Embedded python module (PyEmbed)

- Giordon Holtsberg Stark
   -  quasar with Yocto on a "big" Zynq SoCs: ideas, consulting, development, testing

Dependencies
------------

Note: we have specific instructions for the following operating systems:

- CentOS / RHEL (and dependent distributions):
  - `version 8 <#cc8>`__ (e.g. CentOS 8, RHEL 8, ...),
  - `version 7 <#cc7>`__ (e.g. CentOS 7, RHEL 7, ...).
- `Ubuntu <#ubuntu>`__
- `MS Windows <#windows>`__

For remaining systems (other distros of Linux, other Unix, embedded Linux: PetaLinux, Yocto)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Mandatory:
^^^^^^^^^^

-  C++ compiler with C++11 support (gcc 4.8x is the minimum as of 2022)

-  OPCUA protocol stacks / toolkits (either of them is needed):
   -  see `More info on alternative backends <AlternativeBackends.html>`__.

-  Boost development libraries. (In principle, boost-regex, boost-devel,
   boost-thread and boost-program-options should be sufficient).

-  Python3 (3.6 is what we use at minimum) and some libraries (Jinja2, colorama, lxml, pygit2)

-  cmake3, version 3.10 + (for older versions you could try older quasar
   versions)

-  kdiff3 for occasional merges.

Recommended:
^^^^^^^^^^^^

-  Schema-aware XML editor for editing the design file. We use either of:
   -  Eclipse
   -  VS Code
-  Good C++ development environment (or IDE, ...). The core quasar team uses VS Code.
-  Good merge tool. We use kdiff3 (available as kdiff3 RPM in SLC6
   Linux).
-  ArtisticStyle ( http://astyle.sourceforge.net/ ) for automatic
   formatting of generated code. If you don't use it, the generated code
   will look like a shipwreck (but will still work). Astyle is easy to
   get (e.g. standard in Ubuntu) so get it.
-  graphviz, if you want to visualize your object design in UML-alike
   class diagram.
-  UnifiedAutomation UaExpert or any other OPC UA Client for exploring
   OPC UA address space.
-  Valgrind, for checking memory-related problems
-  PDF viewer for opening UML-like diagrams created from design file.
-  Doxygen for generating documentation.
-  RPM Build tools for RPM generation

Quick setup hints for CentOS 8 (the most standard way)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Install packages from CentOS 8 default repos:

   `` sudo dnf install gcc-c++ gdb boost-devel boost-regex
      boost-program-options boost-signals cmake kdiff3 graphviz valgrind
      doxygen rpm-build xsd openssl-devel libxml2-devel xerces-c-devel
      python3-colorama python3-jinja2``

#. Install packages from epel repo (by default enabled in CentOS 8):

   `` sudo yum install astyle``

#. Download UaExpert for an universal test&dev tool for OPC-UA.

Quick setup hints for CC7/CentOs7 users
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Get Eclipse (www.eclipse.org), preferably the C/C++ Developers
   version (e.g.
   https://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/oxygenr)
   Once installed, install Eclipse add-ons for XML editors
#. Issue this to install software from the official CC7 RPM repos:

   `` sudo yum install gcc-c++ gdb boost-devel boost-regex
      boost-program-options boost-signals cmake3 kdiff3 graphviz
      valgrind doxygen rpm-build xsd openssl-devel libxml2-devel
      xerces-c-devel libgit2 libgit2-devel python36-jinja2 python36-lxml
      python36-colorama``

#. Deploy Python's pygit2 (recommended)
   *WARNING: the version of pygit2 should match the version of
   libgit2-devel! At the time of writing it was 0.26.4*

   ``pip install pygit2==0.26.4 --user``

#. Download UaExpert for an universal test&dev tool for OPC-UA.

Quick setup hints for Ubuntu 20.04
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Deploy from standard Ubuntu repos

   ``sudo apt install cmake g++ libboost-all-dev libxml2-utils astyle
   xsdcxx libxerces-c-dev libssl-dev kdiff3 default-jre libxml2-utils
   python-enum34 python-lxml python3-pip``

#. Deploy Jinja2

   ``pip3 install -U Jinja2 --user``


Quick setup hints for Ubuntu 19.04
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   ``sudo apt install cmake g++ libboost-all-dev libxml2-utils astyle
   xsdcxx libxerces-c-dev libssl-dev kdiff3 default-jre libxml2-utils
   python-enum34 python-lxml``

Setup for Windows users
~~~~~~~~~~~~~~~~~~~~~~~

`<WindowsSetup.html>`__

Tutorial
--------

| quasar has a pretty detailed tutorial for both newcomers as well as
  experienced users.
| The tutorial is available at our YouTube channel:
| https://www.youtube.com/channel/UCQdLb4N-CEWrpYROcC-UF6w
| The relevant playlist:
| https://www.youtube.com/playlist?list=PLz6bxFrT1-KBZxoSxr4ZvlTyxNeYE3L7b

Optional modules
----------------

| quasar is containing a list of built-in (mandatory) modules. A number
  of optional modules exist which can be used for implementing various
  flavours of OPC UA servers. Amongst those are alternative OPC UA
  backends, APIs for device specific protocols or software utilites.
| The list of optional modules is maintained by the quasar team on
  `github <https://github.com/quasar-team/quasar-modules>`__.
| To enable and use a module:
| ./quasar.py enable_module MyQuasarModule [tag] [git server]
| Pulls quasar-modules MyQuasarModule from git, checks version of chosen
  module to be compatible. A module version can be specified, if later a
  new module tag/release is needed, call the command again (the existing
  module version will be removed). You can specify a git server URL e.g.
  to use mirrors in restricted networks (e.g.
  ssh://git@gitlab.cern.ch:7999).
| To download the module and include it in the build configuration run:
| ./quasar.py prepare_build
| cmake will check for new enabled modules and pull from git if they are
  not yet existing. Thereafter you can build your project normally using
  ./quasar.py build which will consider includes and create and link an
  object library called like the module itself.
| Further commands related to optional modules are:
| ./quasar.py remove_module MyQuasarModule Removes downloaded module
  code, if modifications are found - abort.
| ./quasar.py disable_module MyQuasarModule Removes module and disables
  its use (need to enable first again if needed).
| ./quasar.py list_modules [git server] List all maintained modules from
  git and their required quasar versions.
| ./quasar.py list_enabled_modules List all modules enabled in the
  project and their versions.

quasar build system
-------------------

| Quasar build system is `documented here <quasarBuildSystem.html>`__.

Calculated Variables
--------------------

| `User and developer documentation of Calculated
  Variables <../CalculatedVariables/doc/CalculatedVariables.html>`__

Logging
-------

| Logging in quasar servers is provided via the
  `LogIt <https://github.com/quasar-team/LogIt>`__ module.

-  Quasar specific documentation, targeted at quasar developers and
   quasar framework maintainers can be found `here <LogIt.html>`__
-  Generic LogIt documentation can be found
   `here <https://github.com/quasar-team/LogIt>`__

Quick FAQ
---------

#. How to build an executable with debug symbols?

   Just append Debug to your "quasar.py build" invocation, i.e.:

   ``./quasar.py build Debug``

#. Build fails

   Try to read carefully the output.
   If you can't figure the issue on your own, try contacting
   quasar-developers@cern.ch

#. quasar (itself) misbehaves (not to confuse with quasar-made OPC-UA
   components). What can I do?

   #. You can increase verbosity of quasar tooling itself. This might
      point you to a problem (e.g. file permissions issues) or can help
      quasar developers diagnose the problem.
      The verbosity is controlled by environment variable called
      "QUASAR_LOG_LEVEL" and the most relevant levels are "INF" (the
      default, about no debug) and "DBG" (a lot of debug).
      If you use bash, you can increase the verbosity by invoking:

         ``export QUASAR_LOG_LEVEL=DBG``

   #. Sometimes quasar tooling (e.g. anything you run via quasar.py
      command) will terminate with error. We did pay attention to be
      verbose enough but sometimes it might help to drop in the Python
      Debugger (pdb) to debug post-mortem rather than to drop to shell.
      This can be achieved by exporting the variable QUASAR_RUN_PDB to
      1, e.g. in bash:

         ``export QUASAR_RUN_PDB=1``

   #. The general support email is quasar-developers@cern.ch. You can
      get some support there or you can file a bug ticket directly via
      GitHub.

#. How to create a RPM/MSI/DEB package with my OPCUA server?
   CPack fits very well into the provided quasar build system and RPM/MSI are known to work well with quasar.

#. How to upgrade to newer release of quasar?
      #. Download/checkout quasar in the version of your choice
      #. Execute quasar.py upgrade_project which is in its root
         directory, passing path to your project to be upgraded as the
         first parameter:
         ``./quasar.py upgrade_project <path>``

#. Can I use evaluation version of the UA Toolkit to create an OPC UA
   server using this framework?

   You can.
   Note that for many basic features you can use free and open-source
   open62541-compat backend instead of the paid UA Toolkit, see `More info on alternative backends <AlternativeBackends.html>`__.

#. How does a developer define specific command line options for my
   quasar server implementation?
   This is documented here: `User Defined Command Line
   Parameters <UserDefinedCommandLineParameters.html>`__

#. How does a developer define a dynamic (at start up) configuration?
   For example, where the server 'discovers' connected hardware on start
   up.
   This is documented here: `User Defined Runtime
   Configuration <UserDefinedRuntimeConfigurationModification.html>`__

#. How does a developer persist a 'discovered' configuration as
   described above? For example, where the server 'discovers' connected
   hardware initially and saves it for use thereafter as a static
   configuration.
   This is also documented here: `User Defined Runtime
   Configuration <UserDefinedRuntimeConfigurationModification.html#persist>`__

#. How do I link libraries, add include directories, use build
   configurations etc... ? See `build system
   doc <quasarBuildSystem.html>`__.

--------------

Written by Piotr Nikiel <piotr@nikiel.info>
Report inconsistencies and bugs to <quasar-developers@cern.ch>

(C) CERN 2015-. All rights not expressly granted are reserved.
