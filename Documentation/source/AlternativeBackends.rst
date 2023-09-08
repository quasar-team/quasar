OPC-UA back-ends for quasar
===========================

Author: Piotr P. Nikiel

Date: ~2016, rewritten 03-Oct-2022

Support: quasar-developers@cern.ch

quasar for C++ can create OPC UA software components (servers and clients) on top of
OPC UA libraries (SDK - software development kits) which follow the Unified Automation C++ SDK API.

As of the writing, two such libraries exist:

-  `The UA C++
   SDK <https://www.unified-automation.com/products/server-sdk/c-ua-server-sdk.html>`__,
   in different versions, including the evaluation version, detailed below.

-  `The open62541-compat <https://github.com/quasar-team/open62541-compat/>`__, internally using `open62541 <https://open62541.org/>`__,
   both of them being free and open-source, detailed below.

Please note that quasar can also be used for Python-based OPCUA software (in a bit limited fashion),
through `the MilkyWay project <https://github.com/quasar-team/MilkyWay>`__.

Using quasar with the Unified Automation C++ SDK
------------------------------------------------

quasar-made software can be easily build against the UA SDK.
The supported UA SDK version generally depends on versions of quasar used;
2022-made versions (e.g. 1.5.12) are fine with UA SDK 1.5.x, 1.6.x and 1.7.x,
earlier versions also covered UA SDK 1.3.x and 1.4.x.

To achieve the goal, it is only necessary to point quasar to where the UA SDK is,
that is done by either:

* using the ``FrameworkInternals/default_configuration.cmake`` and passing the paths via environment
* writing a new build configuration file

Using quasar with open62541-compat and open62541 (the free and open-source approach)
------------------------------------------------------------------------------------

The quasar team has created a compatibility layer which permits using `open62541
<http://www.open62541.org/>`__ in quasar projects. The compatibility layer is called open62541-compat,
it is free & open-source and quickly available to
quasar developers as one of supported "optional modules".

Activating open62541 is a simple procedure which is best described in the quasar tutorials on YouTube,
see e.g. `<https://www.youtube.com/watch?v=6MsWUQ0OXbY>`__ .

OPC-UA Back-end dependent features: a comparison table
------------------------------------------------------

+-----------------+-----------------+-----------------+-----------------+
| **Feature       |                 | **UA SDK 1.5+   | **open62541-    |
| **              |                 | **              | compat          |
|                 |                 |                 | 1.4.2           |
|                 |                 |                 | **              |
+-----------------+-----------------+-----------------+-----------------+
| **Cache         |                 | ✓               | ✓               |
| variables**     |                 |                 |                 |
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
|                 |                 |                 |                 |
|                 |                 |                 |                 |
|                 |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | Null Data Type  | ✓               | ✓               |
+-----------------+-----------------+-----------------+-----------------+
|                 | Arrays          | ✓               | ✓               |
+-----------------+-----------------+-----------------+-----------------+
|                 | Access rights   | ✓               | ✓               |
|                 | (r/w/rw         |                 |                 |
|                 | control)        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
|                 | No-sampling     | ✓               | x               |
|                 | mode for        |                 |                 |
|                 | cache-variables |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Source        |                 |                 | x               |
| variables**     |                 |                 |                 |
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
|                 | Asynchronous    | ✓               | x               |
|                 | invocation      |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Calculated    |                 | ✓               | ✓               |
| variables**     |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Standardized  |                 | ✓               | ✓               |
| back-end        |                 |                 |                 |
| configuration   |                 |                 |                 |
| ServerConfig.xml|                 |                 |                 |
| **              |                 |                 |                 |
|                 |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

|

|
