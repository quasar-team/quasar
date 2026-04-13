# CLAUDE.md - quasar framework

## What is quasar

quasar (Quick OPC UA Server generAtion fRamework) is a C++/Python code generation framework from CERN. Users declare a server data model in `Design/Design.xml` and quasar generates a complete OPC UA server: C++ address space classes, device logic scaffolding, configuration parsing, and build system integration.

Version: 1.7.2 | License: LGPL v3 / BSD-2 | Website: https://quasar.docs.cern.ch/

## Build and run commands

```bash
# Prerequisites: Python 3 (lxml, Jinja2, colorama), CMake 3.10+, C++ compiler, Boost, xsdcxx

# Select build configuration (required before first build)
./quasar.py set_build_config <path/to/config.cmake>

# Prepare build (merges design, generates code, runs cmake)
./quasar.py prepare_build

# Build (runs prepare_build + compile; default Release, parallel via nproc)
./quasar.py build [Release|Debug]

# Run the server
./build/bin/OpcUaServer --config_file bin/config.xml

# Validate Design.xml against schema + semantic rules
./quasar.py validate_design

# Generate device class stubs
./quasar.py generate device <ClassName>
./quasar.py generate device --all

# Format design XML
./quasar.py format_design

# Generate documentation (HTML or Markdown)
./quasar.py generate as_doc
./quasar.py generate config_doc

# Apply code style (astyle v3+; only runs on committed files)
./quasar.py style_it [filename]

# Module management
./quasar.py list_modules
./quasar.py enable_module <name> [tag]
./quasar.py disable_module <name>

# Check device class override status
./quasar.py device_report

# Show quasar version
./quasar.py quasar_version

# Check external tool availability
./quasar.py external_tool_check
```

## Testing

There is no unit test framework. All testing is integration-level.

```bash
# CI test runner (requires Docker container with OPC UA SDK)
python3 .CI/run_test_case.py \
  --opcua_backend o6 \
  --open62541_compat_branch v1.4.3-rc0 \
  --design .CI/test_cases/<test_name>/Design.xml \
  --config .CI/test_cases/<test_name>/config.xml \
  --generate_all_devices \
  --compare_with_nodeset .CI/test_cases/<test_name>/reference_ns2.xml
```

12 test cases in `.CI/test_cases/`, each with Design.xml + config.xml + reference NodeSet2 XML. Tests build a server, export the address space, and diff against the reference.

CI runs on GitHub Actions (`.github/workflows/ci.yml`): 10 open62541 tests + 7 UASDK tests + 2 AlmaLinux 10 smoke tests (19 total), all in Docker containers.

## Architecture

### Code generation pipeline

```
Design/Design.xml  ──►  DesignInspector (xpath)  ──►  Jinja2 templates  ──►  C++ source
                         Oracle (type mappings)
```

`FrameworkInternals/transformDesign.py` defines 23 transform keys. Each maps a Jinja2 template to a generated output file. The design is merged with `Meta/design/meta-design.xml` at build time to produce `DesignWithMeta.xml`.

### Module map

| Module | Purpose | Generated? |
|---|---|---|
| **Design/** | User's Design.xml + Design.xsd schema | User-written |
| **FrameworkInternals/** | Code gen engine, CLI, validators, build helpers | Framework (Python) |
| **AddressSpace/** | OPC UA nodes, info model, delegating/source variables | Mostly generated (AS\<Class\>) |
| **Device/** | User device logic implementations | Base classes generated; D\<Class\> user-editable |
| **Configuration/** | Runtime XML config parsing, XSD schema generation | Fully generated |
| **Server/** | BaseQuasarServer, QuasarServer (user), OPC UA backends | Hybrid (main.cpp, CMakeLists framework-owned; use ServerCustom.cmake) |
| **Common/** | QuasarThreadPool, shared utilities | Hybrid (CMakeLists framework-owned; use CommonCustom.cmake) |
| **Meta/** | Build info, log level control, thread pool config | Hybrid |
| **CalculatedVariables/** | Formula-based variables (bundled muParser) | Hand-written |
| **LogIt/** | Logging framework (git submodule) | Separate repo |

### Two OPC UA backends

- **open62541** (open-source): active when `FrameworkInternals/EnabledModules/open62541-compat.url` exists
- **UASDK** (Unified Automation, commercial): default when no open62541 module

Selected at CMake configure time. C++ in AddressSpace/Server must compile against both backends or use conditional compilation.

### Runtime entry flow

`Server/src/main.cpp` -> `QuasarServer::startApplication()` -> `BaseQuasarServer` inits OPC UA stack -> `Configurator` loads `config.xml` -> creates AS\<Class\> + D\<Class\> objects -> enters main loop. Default endpoint: `opc.tcp://[NodeName]:4841`.

## Generated vs hand-written code (critical)

### NEVER edit (fully regenerated on every build)

- `Device/generated/*` (Base_D\<Class\>.h, Base_All.cpp, cmake files)
- `AddressSpace/generated/*` (AS\<Class\>.h/.cpp, ASInformationModel, SourceVariables)
- `Configuration/` generated files (Configuration.xsd, Configurator.cpp, ConfigValidator.cpp)
- Any file in a `generated/` directory

### User-owned device files (skip-if-exists)

`Base_D<Class>` (generated) provides virtual methods with safe defaults (log + return `OpcUa_BadNotImplemented`) for all device-logic methods declared in Design.xml. `D<Class>` (user-owned) overrides only the methods it needs, using the `override` keyword.

**D\<Class\>.h** — inherits from `Base_D<Class>`. Override virtual methods as needed. Add custom members and methods below the comment marker. Use `override` keyword on all overridden methods.

**D\<Class\>.cpp** — implement constructor, destructor, and overridden methods. Add fully custom methods at the bottom.

When Design.xml adds new elements, `Base_D<Class>` is regenerated with new virtual defaults. Existing `D<Class>` files are never overwritten — they compile as-is since the base class provides defaults. Run `./quasar.py device_report` to check override status.

Note: older projects may still have legacy section markers (`1111`/`2222`/`3333`) in their D\<Class\>.cpp files. These are harmless comments from the previous merge-based workflow and can be safely removed.

### Always hand-written

- `Design/Design.xml`, runtime config XMLs
- `Server/src/QuasarServer.cpp` (user can override mainLoop, initialize, shutdown)
- All Jinja2 templates (the real source for generated code)
- Framework Python files in `FrameworkInternals/`

## Coding conventions

### C++

- **Naming**: PascalCase classes, camelCase methods/variables. Prefixes: `AS` (address space), `D` (device), `Base_D` (generated base)
- **Namespaces**: `AddressSpace::`, `Device::`, `Meta::` — one per module
- **Include guards**: `#ifndef FILENAME_H_` (not #pragma once, except a few newer files)
- **Error handling**: `UaStatus` return codes for OPC UA SDK operations; `std::runtime_error` exceptions for high-level logic
- **Formatting**: astyle v3+ via `./quasar.py style_it`; no .clang-format
- **License header**: LGPL v3 block (21 lines) with `© Copyright CERN`, author, creation date

### Python

- **Style**: PascalCase classes, snake_case functions, 4-space indent
- **License header**: BSD-2 in docstring with @author, @copyright, @license, @contact
- **No type hints** (older Python style throughout)

### CMake

- **Variables**: `UPPER_CASE_WITH_UNDERSCORES`
- **Targets**: PascalCase (module names: AddressSpace, Server, Device, etc.)
- **Each module**: OBJECT library, linked into single `OpcUaServer` executable

### Commits

- Short imperative style, optional JIRA prefix: `OPCUA-XXXX Description`
- Branch naming: `OPCUA-XXXX_short_description`

## Key files (highest signal)

| File | Why it matters |
|---|---|
| `FrameworkInternals/transformDesign.py` | Defines all 27 code generation targets and template mappings |
| `FrameworkInternals/quasarCommands.py` | Complete CLI command registry |
| `FrameworkInternals/DesignInspector.py` | xpath API over Design.xml; all generators depend on it |
| `FrameworkInternals/Oracle.py` | Type system: OPC UA <-> C++ <-> XSD mappings |
| `Design/Design.xsd` | Schema defining all valid Design.xml constructs |
| `CMakeLists.txt` (root) | Build orchestration, backend selection, module system |
| `Server/src/BaseQuasarServer.cpp` | Server lifecycle and startup flow |
| `FrameworkInternals/DesignValidator.py` | Two-stage validation: XSD + semantic rules |
| `FrameworkInternals/manage_files.py` | File versioning, consistency checks, style_it command |
| `FrameworkInternals/deviceClassAnalyzer.py` | D\<Class\> analysis: `device_report`, `addOverrideToDeviceClasses` |
| `.github/workflows/ci.yml` | CI matrix definition |

## Configuration files

| File | Purpose |
|---|---|
| `Design/Design.xml` | Server data model definition (source of truth) |
| `ProjectSettings.cmake` | Executable name, custom modules, additional libraries |
| `bin/config.xml` | Runtime data configuration (instances of Design classes) |
| `bin/ServerConfig.xml` | OPC UA server settings (endpoints, security, certs, tracing) |
| `Device/DeviceCustom.cmake` | User-owned: custom sources for Device module (`DEVICE_CUSTOM_SOURCES`) |
| `Common/CommonCustom.cmake` | User-owned: custom sources for Common module (`COMMON_CUSTOM_SOURCES`) |
| `Server/ServerCustom.cmake` | User-owned: custom sources for Server module (`SERVER_CUSTOM_SOURCES`) |
| Build config `.cmake` file | Toolchain, Boost/OPC UA paths (set via `set_build_config`) |

## Common pitfalls

- Editing generated files directly — they are overwritten on every build
- Forgetting `--recursive` when cloning (LogIt is a git submodule)
- Changing Jinja2 templates without updating CI reference NodeSet2 files in `.CI/test_cases/`
- Assuming D\<Class\> files are regenerated — they use skip-if-exists; check `device_report` for override status
- Building in-source (CMakeCache.txt in source dir blocks out-of-source builds)
- The `build/` directory is the default `--project_binary_dir`; all generated output goes there
