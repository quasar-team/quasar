           ./quasar.py enable_module open62541-compat OPCUA-2017_UaPropertyCache ;
           ./quasar.py set_build_config open62541_config.cmake ;
           ./quasar.py prepare_build Release ;
           ./quasar.py build Release ;
	   ./.CI/travis/server_fixture.py ;
           exit ;
