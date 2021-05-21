           ./quasar.py enable_module open62541-compat OPCUA-2309_client_side_log_level_control  ;
           ./quasar.py set_build_config open62541_config.cmake ;
           ./quasar.py prepare_build Release ;
           ./quasar.py build Release ;
           ./.CI/travis/server_fixture.py --command_to_run uasak_dump ;
           exit ;
