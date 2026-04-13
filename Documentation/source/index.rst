quasar
======

The quick OPC UA server generation framework.

What is quasar?
---------------

quasar generates complete `OPC UA <https://opcfoundation.org/about/opc-technologies/opc-ua/>`__
servers from a single Design XML file. From that design it produces C++ address
space classes, device logic scaffolding, configuration parsing, and build system
integration -- so you focus on your device logic, not boilerplate.

Developed at `CERN <https://home.cern>`__, quasar is used across multiple
laboratories and experiments to build reliable control and monitoring systems.

Get started
-----------

.. list-table::
   :widths: 30 70
   :header-rows: 0

   * - :doc:`quasar overview <quasar>`
     - Architecture, credits, and version history.
   * - :doc:`Design manual <DesignManual>`
     - Define your server data model in Design XML.
   * - :doc:`Commands reference <quasarCommands>`
     - All ``quasar.py`` commands (build, generate, validate, ...).
   * - :doc:`Build system <quasarBuildSystem>`
     - CMake integration, backends, and project settings.
   * - `YouTube tutorials <https://www.youtube.com/watch?v=v212aPmbahM&list=PLz6bxFrT1-KBZxoSxr4ZvlTyxNeYE3L7b>`__
     - Video walk-throughs from first design to running server.

Key features
------------

- **Design-driven code generation** -- one XML file drives the entire server.
- **Two OPC UA backends** -- open-source `open62541 <https://open62541.org/>`__
  or commercial Unified Automation SDK.
- **Device logic separation** -- generated base classes with safe defaults;
  user code is never overwritten.
- **Module system** -- enable or disable optional components
  (e.g. calculated variables, SNMP via *mule*).
- **Cross-platform** -- Linux (x86-64, ARM64), Windows, and Yocto embedded
  builds.

Contact
-------

| Primary contact: quasar-developers@cern.ch
| Source code: `github.com/quasar-team/quasar <https://github.com/quasar-team/quasar>`__


--------------


.. toctree::
  :hidden:
  :caption: Documentation


		Alternative backends <./AlternativeBackends>
		Calculated variables <./CalculatedVariables>
		ChangeLog <./ChangeLog>
		Design manual <./DesignManual>
		For quasar developers <./forQuasarDevelopers>
		LogIt <./LogIt>
		quasar <./quasar>

		quasar build system <./quasarBuildSystem>
		quasar commands <./quasarCommands>
		User defined command line parameters <./UserDefinedCommandLineParameters>
		User defined runtime configuration modification <./UserDefinedRuntimeConfigurationModification>
		Windows setup <./WindowsSetup>
		Yocto <./yocto>
		Youtube links <./youtube_links>

.. toctree::
  :hidden:
  :caption: Additional files


	External files <./external_files>
	Notes files <./notes_files>


.. toctree::
  :hidden:
  :caption: quasar OPC UA Servers

  Summary of known quasar-based OPC UA servers <./quasar_OPC_UA_servers>
