           ./quasar.py enable_module open62541-compat v1.3.8-rc0 ;
           ./quasar.py set_build_config open62541_config.cmake ;
           ./quasar.py prepare_build Release ;
           ./quasar.py build Release ;
           ./.CI/travis/server_fixture.py --command_to_run uasak_dump ;
           exit ;
