           ./quasar.py enable_module open62541-compat OPCUA-1389_cache_variables_from_datasource_to_variable ;
           ./quasar.py set_build_config open62541_config.cmake ;
           ./quasar.py prepare_build Release ;
           ./quasar.py build Release ;
           ./.CI/travis/server_fixture.py --command_to_run uasak_dump ;
           exit ;
