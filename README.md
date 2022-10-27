# quasar
Quick opcUA Server generAtion fRamework

The project website is [quasar.docs.cern.ch](https://quasar.docs.cern.ch/), where you can obtain more information.

NOTE: Clone quasar with the _--recursive_ flag. Required since quasar uses source code from other git repositories (specifically: [LogIt](https://github.com/quasar-team/LogIt) is a [git submodule](https://git-scm.com/docs/gitsubmodules))
```bash
git clone --recursive https://github.com/quasar-team/quasar
```

## Continuous Integration builds status

CI build info | UA backend | CI build status
------------ | ------------- | -------------
linux (travis-ci) | open62541 | [![travis-ci](https://travis-ci.org/quasar-team/quasar.svg?branch=master)](https://travis-ci.org/quasar-team/quasar?branch=master)
windows (appveyor) | open62541 | [![Appveyor](https://ci.appveyor.com/api/projects/status/q8ruqgd2nj54b76p/branch/master?svg=true)](https://ci.appveyor.com/project/ben-farnham/quasar/branch/master)
