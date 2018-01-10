# LogIt
LogIt is designed to be a high performance, flexible logging solution for C++ projects. The LogIt module builds to a stand-alone shared/static library or directly into any quasar project.

# Overall Architecture
## Front end
LogIt provides a logging front-end; this is the API your code will call to log messages via LogIt. The front-end is fixed, and is provided via the interface file LogIt.h.
## Back ends
LogIt delivers log messages to log repositories via back-ends. LogIt comes with some ready made back-end implementations (fairly standard stuff: stdout, a rolling file buffer based on the boost logging library, etc.) and an open interface for you to provide custom back-end implementations to deliver log messages to your custom log repositories.
## Verbosity
Key functionality in any logging library really, code using LogIt defines the messages that _can_ be logged, each message is annotated with a log level. At runtime, the logging threshold can be raised or lowered - the general idea being that raising the threshold means fewer messages are logged, lowering the threshold means more messages will be logged.
In a production system that is currently behaving well, the logging threshold would probably be left quite high (maybe just errors and warnings), thus keeping the resources spent on logging to a minimum. In a system exhibiting problems, you'll want to diagnose it - i.e. to lower the threshold and allow more verbose logging to understand what the process is actually doing (noting that this means more resources will be spent on logging - processor/disk/...).
