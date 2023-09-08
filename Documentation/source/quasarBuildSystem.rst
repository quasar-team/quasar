Written by: Piotr P. Nikiel <\ piotr@nikiel.info\ > Jan-Feb 2019

Reviewed and improved by: Paris Moschovakos, Ben Farnham, Stefan
Schlenker

.. _h.4qiakb200pno:

quasar, quasar-based servers and the build system
=================================================

quasar is a framework for rapid and efficient development of
(quasar-based) OPC-UA servers in the C++ programming language. A build
system must accompany a framework like quasar to offload its users from
tasks like:

-  compiling the software (i.e. running the compiler on a set of
   sources, passing options, running linkers etc.)
-  guaranteeing coherency of builds (i.e. a changed source file must be
   recompiled, possibly triggering additional steps in the dependency
   chain)
-  guaranteeing efficiency of builds (i.e. it's not necessary to
   recompile a file that has not been changed)
-  handling build portability among different operating systems,
   compilers (including cross compilers), environments, versions of
   required libraries, architectures
-  integrating the server build system as a part of another, parent
   build system.

CMake as quasar's build system workhorse

CMake has been quasar's build system workhorse since quasar's
beginnings. For details on CMake please refer to its webpage,
\ `https://cmake.org/ <https://www.google.com/url?q=https://cmake.org/&sa=D&ust=1585222770886000>`__\ .

The following features of CMake made it an attractive candidate:

-  widespread usage in industry which usually means availability of
   experience, knowledge and developers familiarized with it
-  widespread usage at CERN
-  previous positive experience
-  it's free and it's open-source software
-  support for different operating systems, compilers, architectures.

How CMake is wrapped by quasar and why

For most use-cases, quasar end-users generate compiler specific build
instructions (think unix makefiles, or visual studio solution files) and
invoke the corresponding builds entirely via the standard quasar command
line interface. That is to say, via the command line instruction python
quasar.py build. In these use-cases, the end-user never directly invokes
cmake; quasar’s internal scripts handle this.

The reason for it is that, by quasar design, source files (e.g. Address
Space classes, Device classes…) for different quasar classes live in
source files whose names are dependent on quasar Design.xml and CMake
doesn't support dynamically created lists of files to compile. Some
hacks exist to do this natively in CMake but their robustness and future
availability is questionable.

Therefore quasar.py (quasar's user entry point) wraps the missing part
and:

#. generates Design-dependent list of files for Address Space and Device
   modules,
#. runs CMake in the build (= binary) directory.

However, certain build modes (e.g. Yocto/PetaLinux) require that CMake
is the build entry point. In such a case the actions from point 1 above
can be manually triggered by the user and their deliverables can be
stored in the server repository. Detailed usage mode is discussed in the
Yocto/PetaLinux tutorial of quasar.

How are server sources organized?

Sources of a quasar-based server are organized in modules. Each module
is stored in a separate directory under the quasar server parent
directory. There are three types of modules:

-  native
   These are compulsory modules of any quasar-based server. The build
   system knows about them intrinsically. At the time of writing, they
   comprise of: AddressSpace, CalculatedVariables, Configuration,
   Device, LogIt, Meta and Server.
-  optional
   These are modules which are deployed by quasar's "optional modules"
   mechanism. The build system knows about them because the user selects
   them using quasar.py commands such as enable_module or
   disable_module. A detailed documentation of optional modules is
   available … <place link to HTML>
-  custom
   These are modules which are entirely under user's responsibility, but
   thanks to using the same architecture like aforementioned native and
   optional modules, they can be easily included in the server build
   process.

The architecture defines organization of quasar modules:

-  header files are in include/ (quasar build system will include this
   directory for any enabled module)
-  source files are in src/
-  there is a CMakeLists.txt per each module which is integrated by the
   parent using add_subdirectory
-  other common subdirectories are: doc (documentation), extra
   (supplementary files for additional functionality), py (for python
   code) etc.

Custom module deployment is comprised of:

-  deployment of module sources
   We recommend "git submodules" mechanism for custom modules which are
   shared among multiple projects and direct placement of sources for
   custom modules which are not to be shared among multiple projects.
-  enabling of a module in the build system
   The custom module name should be appended to variable
   CUSTOM_SERVER_MODULES in the file ProjectSettings.cmake 

After both steps the quasar build system will take the custom module
into account.

Summary of build modes available in quasar

+-----------+-----------+-----------+-----------+-----------+-----------+
| Mod       | For       | Toolset   | Build     | Known     | Special   |
| e         | embedded  | (         | entry     | Users     | re        |
|           |           | compiler, | point     |           | striction |
|           |           | linker,   |           |           |           |
|           |           | etc.)     |           |           |           |
+-----------+-----------+-----------+-----------+-----------+-----------+
| Native    | N         | system    | quasar    | Most of   | -         |
| build on  |           | default   | build     | de        |           |
| x64       |           | or user   |           | ployments |           |
|           |           | chosen    |           | for Linux |           |
|           |           | (e.g.     |           | and       |           |
|           |           | LCG)      |           | Windows   |           |
+-----------+-----------+-----------+-----------+-----------+-----------+
| Cross     | Y         | C         | quasar    | No        | open62541 |
| -compiler |           | ontrolled | build     | p         | preferred |
| mode      |           | by        |           | roduction |           |
|           |           | separate  |           | de        |           |
|           |           | toolchain |           | ployments |           |
|           |           | file (CC, |           | now.      |           |
|           |           | CXX,      |           |           |           |
|           |           | sysroot,  |           | Dev for   |           |
|           |           | flags, …) |           | RPi and   |           |
|           |           |           |           | RCE.      |           |
+-----------+-----------+-----------+-----------+-----------+-----------+
| Yocto     | Y         | Yo        | yocto and | Planned   | open62541 |
| mode      |           | cto-built | CM        | pr        | only      |
| (covers   |           |           | ake-level | oduction: |           |
| P         |           |           | in        |           |           |
| etaLinux) |           |           | tegration | gFEX      |           |
|           |           |           | (NO       | mo        |           |
|           |           |           | quasar    | nitoring, |           |
|           |           |           | build)    |           |           |
|           |           |           |           | ELMB++    |           |
|           |           |           |           | receiver  |           |
+-----------+-----------+-----------+-----------+-----------+-----------+
| E         | depends   | Embedder  | 1st step: | eFEX      | -         |
| mbeddable |           | decision  | quasar    | prototype |           |
| server    |           | (e.g. for | build     |           |           |
| (compiles |           | Python    |           |           |           |
| server as |           | match     | 2nd step: |           |           |
| a shared  |           | given     | embedder  |           |           |
| object    |           | Python    | dependent |           |           |
| )         |           | version   |           |           |           |
|           |           |           |           |           |           |
|           |           |        AP |           |           |           |
|           |           | I/ABI...) |           |           |           |
+-----------+-----------+-----------+-----------+-----------+-----------+
| Native    | Y         | system    | quasar    | MuCTPi    | open62541 |
| build but |           | defaul    | build     |           | p         |
| on        |           | t         |           |           | referred, |
| unusual   |           |           |           |           |           |
| arc       |           |           |           |           | possibly  |
| hitecture |           |           |           |           | no        |
| (e.g.     |           |           |           |           | network   |
| ARM)      |           |           |           |           |           |
+-----------+-----------+-----------+-----------+-----------+-----------+

Build configuration files

A build configuration file is a CMake script defining (typically using
the set() statement) the configuration of a build. The aspects typically
covered by build configs are:

-  Path to OPC-UA stack (if used)
-  Path to boost headers and libs and list of libraries to link against
   if they cannot be automatically resolved. Note that by default, the
   quasar cmake engine uses FindBoost to resolve boost header and lib
   locations. To set a custom boost location use the
   BOOST_ROOT environment variable. See also the paragraph about
   ProjectSettings.cmake below.
-  Additional compiler defines (e.g. -fPIC or -Wall or so).

In addition, path(s) to cross-compilers and their sysroot(s) are often
specified there.

You can have (store) multiple build configuration files because one
OPC-UA server project might be built to suit requirements of different
clients. For instance, you can have one build config for Windows Server
2012, one config for Windows 10, one config for Ubuntu and one for
cross-compiling for Raspian.

To switch between different build configs, use "quasar.py
set_build_config" command. Note that "quasar.py build" will always use
the recently chosen build config.

The following optional variables/settings may be used to influence the
build process:

+-----------------------------------+-----------------------------------+
| Setting name                      | Meaning                           |
+-----------------------------------+-----------------------------------+
| IGNORE_DEFAULT_BOOST_SETUP        | By default, quasar tries to       |
|                                   | resolve boost dependencies        |
|                                   | automatically using CMake's       |
|                                   | FindBoost function (see also      |
|                                   | setting                           |
|                                   | ADDITIONAL_BOOST_LIBS below). By  |
|                                   | setting this variable to ON, no   |
|                                   | automatic boost resolution is     |
|                                   | attempted and must be explicitly  |
|                                   | done within the build config.     |
|                                   | Note that if this setting is      |
|                                   | activated, the project setting    |
|                                   | ADDITIONAL_BOOST_LIBS has no      |
|                                   | effect.                           |
+-----------------------------------+-----------------------------------+
| GIT_SERVER_REPLACEMENT            | By default, quasar optional       |
|                                   | modules are fetched from their    |
|                                   | respective github location. Use   |
|                                   | this variable to replace the      |
|                                   | default URL base                  |
|                                   |                                   |
|                                   | https://github.com/               |
|                                   |                                   |
|                                   | with the variable value (example: |
|                                   |                                   |
|                                   | ssh://git@gitlab.cern.ch:7999/    |
|                                   |                                   |
|                                   | ). This allows for example to     |
|                                   | profit from repository mirrors    |
|                                   | within private networks. The      |
|                                   | replacement mechanism can also be |
|                                   | used in other CMake instruction   |
|                                   | sets within quasar project        |
|                                   | modules (see function definition  |
|                                   | in                                |
|                                   | Frame                             |
|                                   | workInternals/UrlHandling.cmake). |
+-----------------------------------+-----------------------------------+

Build configuration files vs ProjectSettings.cmake

The ProjectSettings.cmake file describes project settings which do not
depend on the particular build scenario. At the time of writing the
ProjectSettings.cmake would enable to control the following:

+-----------------------------------+-----------------------------------+
| Setting name                      | Meaning                           |
+-----------------------------------+-----------------------------------+
| CUSTOM_SERVER_MODULES             | List of custom server modules,    |
|                                   | described above.                  |
|                                   | Default: empty.                   |
+-----------------------------------+-----------------------------------+
| EXECUTABLE                        | Name of the executable file.      |
|                                   |                                   |
|                                   | Default: OpcUaServer              |
|                                   |                                   |
|                                   | Note: CMake will automatically    |
|                                   | append platform-dependent suffix, |
|                                   | e.g. .exe for Windows.            |
+-----------------------------------+-----------------------------------+
| SERVER_INCLUDE_DIRECTORIES        | Additional include directories,   |
|                                   | might be evaluated from a CMake   |
|                                   | variable defined in a build       |
|                                   | config.                           |
|                                   |                                   |
|                                   | Default: empty.                   |
+-----------------------------------+-----------------------------------+
| SERVER_LINK_LIBRARIES             | Additional libs to link against,  |
|                                   | might be evaluated from a CMake   |
|                                   | variable defined in a build       |
|                                   | config.                           |
|                                   |                                   |
|                                   | Default: empty.                   |
+-----------------------------------+-----------------------------------+
| SERVER_LINK_DIRECTORIES           | Additional lib directories, might |
|                                   | be evaluated from a CMake         |
|                                   | variable defined in a build       |
|                                   | config.                           |
|                                   |                                   |
|                                   | Default: empty.                   |
+-----------------------------------+-----------------------------------+
| BUILD_SERVER_SHARED_LIB           | If ON, the server will be built   |
|                                   | as a shared object in addition to |
|                                   | a regular executable.             |
|                                   |                                   |
|                                   | Default: OFF                      |
+-----------------------------------+-----------------------------------+

There is always one ProjectSettings.cmake in your project.

CMakeEpilogue.cmake

In certain cases some additional CMake statements must be injected after
the declaration of executable. CMake's INSTALL() statements are a good
example of it.

If that is your case, put those into CMakeEpilogue.cmake. quasar's
master CMakeLists.txt will notice the file and include its content at
the end of master CMakeLists.txt

A good example of such an application is available in
Extra/yocto/CMakeEpilogue.cmake in the quasar repo.

Recommendations to server developers

-  Never modify the master CMakeLists.txt
-  Put relevant settings to ProjectSettings.cmake
-  Use as many build configurations as you need. It is common to reuse
   build configurations between multiple OPC-UA server projects.
