# CI Testing

This directory contains Quasar integration CI test logic:

- `run_test_case.py`: common test runner used by GitLab CI jobs.
- `server_fixture.py`: starts/stops `OpcUaServer` and runs `uasak_dump`.
- `test_cases/`: design/config/reference fixtures for scenario tests.

## Test Case Coverage

Test case | open62541 | UASDK | Nodeset compare
--- | --- | --- | ---
`default_quasar_design` | yes | yes | yes
`cache_variables` | yes | yes | yes
`methods` | no | yes | yes
`async_methods` | no | yes | yes
`source_variables` | no | yes | yes
`recurrent_hasobjects` | yes | yes | no
`single_variable_node` | yes | yes | no
`instantiation_from_design` | yes | yes | yes
`config_entries` | yes | yes | yes
`config_restrictions` | yes | yes | no
`calculated_variables` | yes | yes | yes
`defaulted_instance_name` | yes | yes | yes
