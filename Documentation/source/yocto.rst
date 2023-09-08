Quasar + Yocto: How to (applies to PetaLinux, too)
==================================================

| 
| Author: Piotr Nikiel
| Major revision date: 03-Dec-2018
| Minor revision date: 03-Sep-2019

Introduction
------------

| 
| This rather short how-to explains how to integrate a Quasar-based
  OPC-UA server with Yocto.
| Prerequisites:

-  familiarity with Quasar: creating Quasar projects, working with
   Design, attaching custom code, storing them on git. Experience with
   writing build config files for Quasar (involving cross-compiler
   toolchain) is a plus.
-  familiarity with Yocto: building images for qemu and/or real
   hardware, deploying layers and recipes.

| 
| The how-to presented here has been attempted in:

-  Yocto, different versions (including Rocko and Sumo),
-  PetaLinux 2017.4, which uses Yocto behind the scenes, to build OPC-UA
   server for a PetaLinux-based project.
   Note that only few points of those presented here will be of use for
   PetaLinux integration, especially the recipes will be of value.

The terminology
---------------

| Note that I used my personal paths:

-  ~/gitProjects/quasar
   is where I have my quasar repo (i.e. a git clone from
   https://github.com/quasar-team/quasar). I will call it **quasar
   repo**.
-  ~/gitProjects/poky
   is where I have my reference Yocto distribution usually called poky
   (in this manual it was in the krogoth version). I cloned it from
   Yocto git repo. I will call it **poky repo**.
-  ~/gitProjects/poky-quasar
   is where I will have my Quasar-based OPC-UA server project. I will
   call it **project repo**.

The procedure
-------------

Make sure you're using the Quasar branch with Yocto support
-----------------------------------------------------------

| Yocto and PetaLinux support should work in any released quasar version
  from 1.3.0 on.
| From quasar repo:

.. container::

   git checkout v1.3.4

Create a new Quasar project
---------------------------

| From quasar repo:

.. container::

   ./quasar.py create_project ~/gitProjects/poky-quasar

Put your new Quasar project under git version control
-----------------------------------------------------

.. container::

   cd ~/gitProjects/poky-quasar
   git init .
   git add \*
   git commit -m 'Initial commit'

Copy the reference build config from quasar repo, select it and store it
------------------------------------------------------------------------

| From project repo:

.. container::

   cp ~/gitProjects/quasar/Extra/yocto/yocto_open62541_config.cmake .
   ./quasar.py set_build_config yocto_open62541_config.cmake
   git add yocto_open62541_config.cmake
   FrameworkInternals/build_config_selector.cmake
   git commit -a -m 'Added build config'

Add CMake epilogue with install statements
------------------------------------------

| 
| We need to instruct CMake which files to install on the target. This
  can be done by copying CMakeEpilogue.cmake from us and storing it in
  your project repo:

.. container::

   cp ~/gitProjects/quasar/Extra/yocto/CMakeEpilogue.cmake .
   git add CMakeEpilogue.cmake
   git commit -a -m 'Added the epilogue'

Generate CMake headers and store them
-------------------------------------

| From project repo:

.. container::

   ./quasar.py generate cmake_headers
   git add AddressSpace/cmake_generated.cmake -f
   git add Device/generated/cmake_header.cmake -f
   git commit -m 'Added CMake headers'

Comment: this step is normally done behind the scenes by quasar when its
usual entry point (quasar.py) is used rather than plain CMake. **That's
why -f (force) is used.
**

Force to use host machine JRE
-----------------------------

| From project repo, replace value for key "java" in
  FrameworkInternals/commandMap.py to point directly to your host system
  JRE, for example for CentOS 7 or Ubuntu it would be:
| "java":"/usr/bin/java",
| Comment: we know that this bends Yocto policy a bit but at the time of
  writing (July 2018) we could not get to build OpenJDK Native in Yocto
  without GNU Classpath which seems not mature enough to run the Saxon
  XSLT processor.

Enable open62541-compat module
------------------------------

| Note: you can also try other versions of open62541-compat; we can't
  guarantee this document to be always up to date.
| From project repo:

.. container::

   ./quasar.py enable_module open62541-compat v1.1.2
   git add FrameworkInternals/EnabledModules/open62541-compat.\*
   git commit -m 'Stored open62541-compat'

Check if project builds for your host system
--------------------------------------------

| 
| Comment: this step is not necessary per se but it will help to ensure
  that things are fine so far.
| From project repo:

.. container::

   ./quasar.py build

| If you see:
| [100%] Built target OpcUaServer
| among the last lines of output then you're good.

Create a branch in Yocto reference distribution for your developments
---------------------------------------------------------------------

| Comment: you might skip this step if you are not starting from 'ground
  zero'.
| From poky repo:

.. container::

   git branch my-yocto-dev
   git checkout my-yocto-dev

| 

Source Yocto's oe-init-build-env 
---------------------------------

| From poky repo:

.. container::

   . oe-init-build-env

Build core-image-minimal (in case it's your first Yocto build)
--------------------------------------------------------------

| This step will most likely cost you few hours to execute - but it's
  the one-time investment you have to take.
| Yocto will pull all the dependencies and build basic things (the
  compiler, some basic OS image etc.
| From poky/build dir:

.. container::

   bitbake core-image-minimal

Create a new Yocto layer for your quasar OPC-UA server(s)
---------------------------------------------------------

| From poky repo:

.. container::

   bitbake-layers create-layer meta-quasar-servers
   cd build
   bitbake-layers add-layer ../meta-quasar-servers

| 

Deploy the Yocto recipe for your OPC-UA server
----------------------------------------------

| 
| From meta-quasar-servers which you created inside poky repo:

.. container::

   mkdir recipes-opcua-servers
   cd recipes-opcua-servers
   mkdir opcua-servers
   cd opcua-servers

| And copy my-opcua-server.bb from quasar repo / Extra / yocto.
| This is an example recipe which you  might base on.
| **Note:** don't put underscore ( \_ ) sign as a part of your recipe
  name, it serves as a delimeter between various fields of recipe
  filename. Prefer hyphens ( - ).

Fix the path in the my-opcua-server recipe
------------------------------------------

| 
| The source path have to point to where your actual git repository of
  the project is (and we assumed in point 2 that it is a local one).
| The variable to edit is called **SRC_URI**

Deploy xsd Yocto recipe (if not provided by anything else)
----------------------------------------------------------

| 
| Out of standard Quasar dependencies, xsd is one of those not covered
  by the default layers.
| If you don't provide xsd via other recipes of your system (which is
  most likely to happen when you are starting with Yocto) take the
  recipe from our repo like in the previous point.

Deploy python-enum34 if unavailable in your Yocto/PetaLinux
-----------------------------------------------------------

| For example, PetaLinux 2017.4 provides "enum34" without "native"
  extension. You can use our recipe to provide yourself with a fix.

Deploy pygit2 if you want to have your build stamp embedded in the build (and available via address-space)
----------------------------------------------------------------------------------------------------------

| 
| If pygit2 is available at the OPC-UA server build time then later on,
  from the address-space as well as command line you'll be able to check
  which project commit has been used to produce the executable.
| In case you don't have pygit2 via your Yocto/PetaLinux you can use the
  recipe we prepared for you in the quasar/Extra/yocto directory.

Fix python-six if necessary
---------------------------

| In certain cases (e.g. PetaLinux 2017.4) we've found that python-six
  recipe (required to build open62541 0.3+ which is an "automatic"
  dependency of quasar) is broken by requiring python-io (which is an
  embedded feature of Python thus doesn't need a separate recipe).
| If that is the case for you, you can copy python-six recipe from the
  SDK (Yocto/PetaLinux) to your recipes-apps, remove python-io from
  RDEPENDS and change the layer priority in order to overwrite the
  broken recipe.
| The priority of a layer can be configured via layer.conf file, for
  PetaLinux this normally will be in:

.. container::

   ./project-spec/meta-user/conf/layer.conf

| 
| but you can generally look for it using:

.. container::

   grep -HnRsi BBFILE_PRIORITY .

| 

Deploy meta-openembedded layer to get some required Python modules
------------------------------------------------------------------

| In poky repository:
| For Yocto sumo:

.. container::

   git clone git://git.openembedded.org/meta-openembedded -b sumo
   --depth=1
   cd build
   bitbake-layers add-layer ../meta-openembedded/meta-oe
   bitbake-layers add-layer ../meta-openembedded/meta-python

| For Yocto krogoth:

.. container::

   git clone git://git.openembedded.org/meta-openembedded -b krogoth
   --depth=1
   cd build
   bitbake-layers add-layer ../meta-openembedded/meta-oe
   bitbake-layers add-layer ../meta-openembedded/meta-python

Add your OPC-UA server artifacts to be installed on the target image
--------------------------------------------------------------------

| 
| From poky repo, edit your build/conf/local.conf and add the following
  statement in the end:
| IMAGE_INSTALL_append = " my-opcua-server"
| Note that the space character after double-quote is intentional.

Build the minimal image
-----------------------

| At this stage, you could execute from poky/build directory:

.. container::

   bitbake core-image-minimal

| You can also build only the OPC-UA server:

.. container::

   bitbake my-opcua-server

Run the minimal image in qemu
-----------------------------

| From poky/build directory:

.. container::

   runqemu qemux86 core-image-minimal

| You should see your operating system bootin in qemu.

Run the OPC-UA server
---------------------

| Once you see login prompt in qemu, login as root.
| Then:

.. container::

   cd /opt/QuasarServer
   ./OpcUaServer

| 
| **Note: if you see an error that Configuration.xsd can't be found,
  just edit (vi) the config file making sure that Configuration.xsd has
  no path prefix**.

Connect to the OPC-UA server using UaExpert
-------------------------------------------

| 
| QEMU normally opens a bridge network interface between the host
  machine and the simulated target. In my case I see it when starting
  qemu (point 15) - look at the part in bold:
| /home/pnikiel/gitProjects/poky/build/tmp/sysroots/x86_64-linux/usr/bin/qemu-system-i386
  -kernel images/qemux86/bzImage-qemux86.bin -net nic,model=virtio -net
  tap,vlan=0,ifname=tap0,script=no,downscript=no -cpu qemu32 -drive
  file=/home/pnikiel/gitProjects/poky/build/tmp/deploy/images/qemux86/core-image-minimal-qemux86-20180601101954.rootfs.ext4,if=virtio,format=raw
  -show-cursor -usb -usbdevice tablet -vga vmware -no-reboot -m 256
  -serial mon:vc -serial null -append "vga=0
  uvesafb.mode_option=640x480-32 root=/dev/vda rw mem=256M
  ip=\ **192.168.7.2**::192.168.7.1:255.255.255.0 oprofile.timer=1
  rootfstype=ext4 "
| You can therefore open UaExpert and connect to the endpoint at
  192.168.7.2:4841.
