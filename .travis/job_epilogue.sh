           python quasar.py enable_module open62541-compat ;
           python quasar.py set_build_config open62541_config.cmake ;
           python quasar.py prepare_build Release ;
           python quasar.py build Release ;
           exit ;
