<p align="center">
  <img src="Documentation/source/_static/quasar_logo.png" alt="quasar logo" width="100">
</p>
<h1 align="center">quasar</h1>
<p align="center">Quick OPC UA Server Generation Framework</p>

<p align="center">
  <a href="https://github.com/quasar-team/quasar/actions/workflows/ci.yml"><img src="https://github.com/quasar-team/quasar/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://github.com/quasar-team/quasar/actions/workflows/deploy_documentation.yml"><img src="https://github.com/quasar-team/quasar/actions/workflows/deploy_documentation.yml/badge.svg" alt="Deploy documentation"></a>
  <a href="https://github.com/quasar-team/quasar/releases/latest"><img src="https://img.shields.io/github/v/release/quasar-team/quasar" alt="GitHub Release"></a>
  <a href="https://www.gnu.org/licenses/lgpl-3.0"><img src="https://img.shields.io/badge/License-LGPL_v3-blue.svg" alt="License: LGPL v3"></a>
  <a href="https://quasar.docs.cern.ch"><img src="https://img.shields.io/badge/docs-quasar.docs.cern.ch-brightgreen" alt="Documentation"></a>
</p>

---

quasar generates complete [OPC UA](https://opcfoundation.org/about/opc-technologies/opc-ua/) servers from a single Design XML file. From that design it produces C++ address space classes, device logic scaffolding, configuration parsing, and a CMake build system -- so you focus on your device logic, not boilerplate.

Developed at [CERN](https://home.cern), quasar is used across multiple laboratories and experiments to build reliable control and monitoring systems.

## Quick start

### Clone

```bash
git clone --recursive https://github.com/quasar-team/quasar
```

The `--recursive` flag is required -- quasar uses [LogIt](https://github.com/quasar-team/LogIt) as a git submodule.

### Build

```bash
./quasar.py set_build_config <path/to/config.cmake>
./quasar.py build
```

### Run

```bash
./build/bin/OpcUaServer --config_file bin/config.xml
```

## How it works

```text
Design/Design.xml
    |
    v
DesignInspector (xpath) + Oracle (type mappings)
    |
    v
Jinja2 templates (23 transforms)
    |
    v
Generated C++ source (AddressSpace, Device bases, Configuration, CMake)
```

`Design.xml` is the single source of truth. Generated code goes into `AddressSpace/`, `Device/generated/`, and `Configuration/`. User-owned device logic lives in `D<Class>` files which are never overwritten.

## Features

- **Design-driven code generation** -- one XML file drives the entire server
- **Two OPC UA backends** -- open-source [open62541](https://open62541.org/) or commercial Unified Automation SDK
- **Device logic separation** -- generated base classes with safe defaults; user code is never overwritten
- **Built-in validation** -- XSD schema checks plus semantic rules on your design
- **Module system** -- enable or disable optional components (calculated variables, archiving, etc.)
- **Cross-platform** -- Linux (x86-64, ARM64), Windows, and Yocto embedded builds

<details>
<summary>Prerequisites</summary>

- Python 3 with lxml, Jinja2, colorama
- CMake 3.10+
- C++17 compiler
- Boost
- xsdcxx (CodeSynthesis XSD)

</details>

## Documentation

Full documentation is available at **[quasar.docs.cern.ch](https://quasar.docs.cern.ch/)**, covering the [design manual](https://quasar.docs.cern.ch/DesignManual.html), [command reference](https://quasar.docs.cern.ch/quasarCommands.html), [build system](https://quasar.docs.cern.ch/quasarBuildSystem.html), and [video tutorials](https://www.youtube.com/watch?v=v212aPmbahM&list=PLz6bxFrT1-KBZxoSxr4ZvlTyxNeYE3L7b).

## License

This project is licensed under the GNU Lesser General Public License v3.0. See [LICENSE](LICENSE) for details.

## Contact

Primary contact: quasar-developers@cern.ch
