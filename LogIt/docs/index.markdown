# LogIt Documentation
LogIt is designed to be a high performance, flexible logging solution for C++ projects. The LogIt module builds to a stand-alone shared/static library or directly into any quasar project.

# Overall Architecture
## Front end
LogIt provides a logging fron-end; this is the API your code (using LogIt) will call to log messages. The front-end is fixed, and is provided via the interface file LogIt.h.
## Back ends
LogIt delivers log messages to log repositories via back-ends. LogIt comes with some ready made back-end implementations (standard stuff: stdout, a rolling file buffer based on the boost logging library, etc.) and an open interface for you to provide custom back-end implementations to deliver log messages to your custom log repositories.
