# quasar
Quick opcUA Server generAtion fRamework

NOTE: Clone quasar with the _--recursive_ flag. Required since quasar uses source code from other git repositories (specifically: (LogIt)[https://github.com/quasar-team/LogIt] is a (git submodule)[https://git-scm.com/docs/gitsubmodules])
```bash
git clone --recursive https://github.com/quasar-team/quasar
```

## Out-of-the-box dependencies.
Once you've cloned the quasar framework git repository to your development machine you'll need to obtain some dependencies before you can start developing. Due to licensing, these dependencies can't be distributed with the quasar code, but please read _quasar-checkout-dir_/Documentation/GenericServer.html for detail.

## Continuous Integration builds status (master branch)

CI build info | UA backend | CI build status
------------ | ------------- | -------------
linux (travis-ci) | open62541 | [![travis-ci](https://travis-ci.org/quasar-team/quasar.svg?branch=master)](https://travis-ci.org/quasar-team/quasar?branch=master)
windows (appveyor) | open62541 | [![Appveyor](https://ci.appveyor.com/api/projects/status/q8ruqgd2nj54b76p/branch/master?svg=true)](https://ci.appveyor.com/project/ben-farnham/quasar/branch/master)
