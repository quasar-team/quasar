import os
import sys
import json

QUASAR_PASS = os.environ.get('QUASAR_PASS')

JSON_FILE = './Documentation/source/_static/versions.json'
JSON_FILE_LOCAL = '/home/quasar/quasar/versions.json'

VERSIONS_FOLDER = '/usr/share/nginx/quasar/version'
EOS_VERSIONS_FOLDER = '/eos/project-q/quasar/www/version'

if __name__ == '__main__':
  cp_local_command = f'cp {JSON_FILE} {JSON_FILE_LOCAL}'
  print(f'Copying last version.json to local folder: ', cp_local_command)

  for folder_version in os.listdir(VERSIONS_FOLDER):
    print('Processing version: ', folder_version)
    
    cp_nginx_command = f'echo \"{QUASAR_PASS}\" | sudo -S cp {JSON_FILE_LOCAL} {VERSIONS_FOLDER}/{folder_version}/_static/versions.json'
    os.system(cp_nginx_command)
    
    cp_eos_command = f'cp {JSON_FILE_LOCAL} {EOS_VERSIONS_FOLDER}/{folder_version}/_static/versions.json'
    os.system(cp_eos_command)

  exit(0)
