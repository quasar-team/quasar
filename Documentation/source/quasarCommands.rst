quasar.py commands help
=======================

.. image:: images/quasar_engine_small_smooth_square_128x128.png
   :width: 128px
   :height: 128px

build
-----

| This is the main command to build the project (a quasar-based OPC-UA
  server).
| Syntax:

.. container:: syntax

   ./quasar.py build [--builder builder] [buildType]

| 

| Where:

-  builder (optional) can be Ninja (in Linux), or left absent for
   whatever is your system's default (typically GNU Make in Linux and
   MSVC build in Windows).
-  buildType (optional) can be either Debug or Release. When not
   specified it defaults to Release.

build_config
------------

| Prints the currently chosen build configuration file.

See also `set_build_config <#set_build_config>`__.

check_consistency
-----------------

| Checks the consistency of the project in terms of quasar project
  structure.
| The following classes of errors might be reported:

-  user modification of a file which belongs to quasar (found by
   checksum verification),
-  broken structure of quasar project (e.g. missing files),
-  file(s) not committed in the version control system,

clean
-----

| Deprecated because quasar does out-of-source builds by default for
  long time.
| Simply remove your build directory.

create_project
--------------

| Creates a new quasar project at path given as *destination*.

| Syntax:

.. container:: syntax

   ./quasar.py create_project <destination>

create_release
--------------

| *Warning: this command is only for the developers of quasar itself.*

| Upgrades files.txt with the contents of original_files.txt. Expert
  command, only to be used by developers of the framework when creating
  a new release.
| This command is strictly reserved to the quasar heads and it won't
  work in a typical user environment.

doxygen
-------

|  Runs doxygen in the documentation folder, by default profiting from
  quasar-supplied Doxyfile.

enable_module
-------------

Enables optional module from quasar's optional modules catalogue. The
catalogue is fetched from GitHub (or its local mirror) in case it is
necessary.

| Syntax:

.. container:: syntax

   ./quasar.py enable_module <moduleName> [tag] [serverString]

| Where:

-    moduleName - the name (you can list available modules by
   `list_modules <#list_modules>`__)
-    tag (optional) - tag/branch to checkout, if not specified, master
   branch is used
-    serverString (optional) - default git server is
   "https://github.com", specify custom if necessary, e.g.
   "ssh://git@gitlab.cern.ch:7999"

disable_module
--------------

Disables optional module. Module files will be deleted.

| Syntax:

.. container:: syntax

   ./quasar.py disable_module <moduleName>

external_tool_check
-------------------

| Checks dependencies of quasar.

format_design
-------------

Formats/styles Design.xml.

| The backup is left as Design.xml.backup.

generate as_doc
---------------

| Generates address space documentation (the documentation of all OPCUA
  ObjectTypes, Variables, etc... ) and places it in
  Documentation/AddressSpaceDoc.html.

generate asclass
----------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

| Generates the files AS<classname>.h and AS<classname>.cpp.

generate base
-------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

| Generates the files Base_D<classname>.h and Base_D<classname>.cpp.

generate cmake_headers
----------------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

| Generates parts of CMakeLists.txt which need to be derived from
  Design.

generate config_cpp
-------------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

| Generates the file Configurator.cpp.

generate config_doc
-------------------

| Generates documentation of the Configuration Schema.
| Such a documentation is a HTML document with description of all fields
  that might appear in the configuration.

generate config_xsd
-------------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

| Generates the file Configuration.xsd. This method is called
  automatically by cmake, it does not need to be called by the user.

generate config_validator
-------------------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

| Generates the file ConfigValidator.xsd. This method is called
  automatically by cmake, it does not need to be called by the user.

generate device
---------------

Generates the Device Logic files D<classname>.h and D<classname>.cpp.

| Syntax:

.. container:: syntax

   ./quasar.py generate device <quasar class>

generate device --all
---------------------

| Equal to "generate device" (above) run with all classes with device
  logic from the Design.

generate diagram
----------------

| Creates a *quasar design diagram* for this project.

| Syntax:

.. container:: syntax

   ./quasar.py generate diagram [detailLevel [mode]]

| 
|    
| Where:

-  detailLevel is between 0 and 4, and it defaults to 0. When the
   detailLevel is increased you see more and more information.
-  mode defaults to "dot" which is the most hierarchical graphviz
   operation mode. You can also try "fdp" and "circo", for mixed
   results.

| If bored with the typical diagrams, try:

.. container:: syntax

   ./quasar.py generate diagram 4 circo

| 

generate honkytonk
------------------

| Generates honkyTonky.cc, a special class intended for testing your
  namespace with simulated (random) values.

generate info_model
-------------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.
*

| Generates the files ASInformationModel.h and ASInformationModel.cpp.

 generate source_variables
--------------------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

.. _section-1:

|  Generates the files SourceVariables.h and SourceVariables.cpp.

generate root
-------------

*Warning: this command is internally called by the build system and it
is not expected to be directly used by users.*

| Generates the files DRoot.h and DRoot.cpp.

list_enabled_modules
--------------------

| List all optional modules that were enabled for your project.

list_modules
------------

| List all officially registered quasar optional modules.

prepare_build
-------------

| *Warning: used to be user's command but its future is debatable -
  don't use.*

quasar_version
--------------

| Prints currently deployed quasar version.

set_build_config
----------------

| Sets the new build config. Some standard build configs are shipped
  with quasar, e.g. `open62541 build
  config <https://github.com/quasar-team/quasar/blob/master/open62541_config.cmake>`__
  for building against open62541 (via open62541-compat).

| See also `build_config <#build_config>`__

setup_svn_ignore
----------------

| If you use SVN as your version control system, it setups the
  .svnignore file, so the generated files will be ignored in your svn
  repository.

symlink_runtime_deps
--------------------

New in quasar 1.3.0.

| When you build out-of-source (which is the default since quasar 1.3.0)
  your executable will be done in build/bin (the default) but some files
  (like ServerConfig.xml) which are normally in bin/ will be needed to
  run it.
| This convenience function can be run:

-  with no arguments -> symlinks ServerConfig.xml and config*.xml
-  with one argument -> symlinks whatever wildcard-matches the argument,
   from bin/ in the source directory to bin/ in the build directory
   (NOTE: don't forget to escape asterisk if you use it by backslash!)

upgrade_design
--------------

| upgrade_design is used to cover backwards-incompatible Design changes
  between different versions of quasar.
| As of quasar 1.3.4, it can be run with the following options:

-  convert_to_hasDeviceLogic=yes
   when you are upgrading from quasar older than 1.1.0,
-  add_nullPolicy=nullAllowed or add_nullPolicy=nullForbidden
   when you are upgrading from quasar older than 0.96

upgrade_project
---------------

| Upgrades the framework in a given target directory (where a quasar
  project already is).

| Syntax:

.. container:: syntax

   ./quasar.py upgrade_project <destination>

validate_design
---------------

| Validates your Design using built-in Design Validator.

| You can run it on your own though each invocation to build will also
  run it.
