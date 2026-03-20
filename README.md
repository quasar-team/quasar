# quasar
Quick opcUA Server generAtion fRamework

The project website is [quasar.docs.cern.ch](https://quasar.docs.cern.ch/), where you can obtain more information.

NOTE: Clone quasar with the _--recursive_ flag. Required since quasar uses source code from other git repositories (specifically: [LogIt](https://github.com/quasar-team/LogIt) is a [git submodule](https://git-scm.com/docs/gitsubmodules))
```bash
git clone --recursive https://github.com/quasar-team/quasar
```

## Continuous Integration

Quasar CI is driven by GitLab pipeline definitions in `ci/gitlab/`.
Integration test fixtures and runner logic are located in `ci/testing/`.
