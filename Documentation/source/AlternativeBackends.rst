Alternative OPC-UA back-ends for quasar
=======================================

| 
| Here, the term "OPC-UA back-end" refers to a piece of software, which
  provides an API compatible with the Unified Automation C++ SDK (note
  not all features of the UA API are actually used in quasar).
| Quasar supports multiple OPC-UA back-ends.
| As of June 2016, the following backends are supported in quasar:

-  `The UA C++
   SDK <https://www.unified-automation.com/products/server-sdk/c-ua-server-sdk.html>`__,
   version 1.3.x, 1.4.x, 1.5.x.
   Note that you can use the evaluation version of the UA C++ SDK in
   case you don't have the regular license.
   Note this back-end works directly in quasar and we don't support any
   further instructions.
-  The `open62541 <https://open62541.org/>`__ used through
   `open62541-compat <https://github.com/quasar-team/open62541-compat/>`__
   Read below.

| 

Using open62541 (open-source OPC-UA stack)
------------------------------------------

| The open62541 is a C-based OPC-UA stack and on its own it can't be
  used as a back-end for Quasar.

| Therefore, the Quasar team has created a compatibility layer which
  lets use open62541 in quasar projects. The compatibility layer is
  quickly available to quasar developers as one of supported "optional
  modules".

| Note that the procedure requires to posses the build config file for
  open62541 - at the moment, you can obtain it from Piotr.Nikiel@cern.ch
| The quasar team is working on a more sustainable way to distribute it.

| In order to configure your quasar project to use the open62541, follow
  the steps (it is assumed you already have a quasar project):

#. Enable open62541-compat in your project
   ./quasar.py enable_module open62541-compat
#. Set build configuration for open62541
   ./quasar.py set_build_config <your open62541-config file>
#. Download all required quasar modules
   ./quasar.py prepare_build
#. Prepare open62541-compat (it will download and compile open62541
   itself)
   cd open62541-compat/
   python prepare.py
   cd ..
#. Run the build:
   ./quasar.py build

| 

Configuring open62541: endpoint urls, etc.
------------------------------------------

| Quasar makes full use of UA SDK ServerConfig.xml to configure things
  like:

-  endpoint(s) to open (including port numbers, etc)
-  Application/Product URI(s) - relevant when discovery service is used
-  Discovery servers to register on
-  Behavioural settings, like the max polling frequencies, etc.

| Unfortunately, support for ServerConfig.xml is not yet there when the
  open62541 is used as a backend. The ticket to follow is:

| https://its.cern.ch/jira/browse/OPCUA-686

| Changing the defaults must generally be hardcoded, e.g.  look at the
  constructor in Server/src/opcserver_open62541.cpp.
| m_server_config is the object storing the configuration for the server
  when open62541 is used.

| 

OPC-UA Back-end dependent features: a comparison table
------------------------------------------------------

+-----------------+-----------------+-----------------+-----------------+
| **Feature       |                 | **UA SDK 1.3+   | **o             |
| **              |                 | **              | pen62541-compat |
|                 |                 |                 | (TODO: version) |
|                 |                 |                 | **              |
+-----------------+-----------------+-----------------+-----------------+
| **Ca            |                 |                 |                 |
| che-variables** |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Data Types:     | ✓               | ✓               |
|                 | Boolean, SByte  |                 |                 |
|                 | (~signed char), |                 |                 |
|                 | Byte (~unsigned |                 |                 |
|                 | char), Int16,   |                 |                 |
|                 | UInt16, Int32,  |                 |                 |
|                 | UInt32, Int64,  |                 |                 |
|                 | UInt64, Float,  |                 |                 |
|                 | Double, String, |                 |                 |
|                 | ByteString      |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Variant Data    | ✓               | ✓               |
|                 | Type            |                 |                 |
|                 | (supporting the |                 |                 |
|                 | data types      |                 |                 |
|                 | listed above)   |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Null Data Type  | ✓               | ✓               |
+-----------------+-----------------+-----------------+-----------------+
|                 | Access rights   | ✓               | x               |
|                 | (r/w/rw         |                 |                 |
|                 | control)        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | No-sampling     | ✓               | x               |
|                 | mode for        |                 |                 |
|                 | cache-variables |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Sou           |                 |                 | x               |
| rce-variables** |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Support for     | ✓               | x               |
|                 | same data types |                 |                 |
|                 | as with         |                 |                 |
|                 | cache-variables |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Synchronous     | ✓               | x               |
|                 | read/write      |                 |                 |
|                 | handling        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Asynchronous    | ✓               | x               |
|                 | read/write      |                 |                 |
|                 | handling        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Methods**     |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Support for     | ✓               | ✓               |
|                 | same data types |                 |                 |
|                 | as with         |                 |                 |
|                 | cache-variables |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Synchronous     | ✓               | ✓               |
|                 | invocation      |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Asynchronous    | x               | x               |
|                 | invocation      |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Calculated    |                 | (broken)        | x               |
| items**         |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Standardized  |                 | ✓               | x               |
| back-end        |                 |                 |                 |
| configuration   |                 |                 |                 |
| (Se             |                 |                 |                 |
| rverConfig.xml) |                 |                 |                 |
| **              |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

| 

| 
