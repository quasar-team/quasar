           echo '********************************************************************' ;
           echo branch ${TRAVIS_PULL_REQUEST_BRANCH:-$TRAVIS_BRANCH} ;
           echo '********************************************************************' ;
           apt install python-enum34 ;
           ln -s /usr/bin/cmake /usr/bin/cmake3 ;
           git config --global user.email quasar-developers@cern.ch ;
           git config --global user.name QuasarDeveloper ;
           git clone --recursive -b ${TRAVIS_PULL_REQUEST_BRANCH:-$TRAVIS_BRANCH} https://github.com/quasar-team/quasar.git ;
           cd quasar ;
