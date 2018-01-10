# LogIt
LogIt is designed to be a high performance, flexible logging solution for C++ projects

# Overall Architecture
## Front end
LogIt provides a logging front-end; this is the API your code will call to log messages via LogIt. The front-end is fixed, and is provided via the interface file LogIt.h.

_example: logging a message with the LogIt API_
```
LOG(Log::INF) << "hello, world!");
```
## Back ends
LogIt delivers log messages to log repositories via back-ends. LogIt comes with some ready made back-end implementations (fairly standard stuff: stdout, a rolling file buffer based on the boost logging library, etc.) and an open interface for you to provide custom back-end implementations to deliver log messages to your custom log repositories.
## Verbosity
Key functionality in any logging library really, code using LogIt defines the messages that _can_ be logged, each message is annotated with a log level. At runtime, the logging threshold can be raised or lowered - the general idea being that raising the threshold means fewer messages are logged, lowering the threshold means more messages will be logged.
In a production system that is currently behaving well, the logging threshold would probably be left quite high (maybe just errors and warnings), thus keeping the resources spent on logging to a minimum. In a system exhibiting problems, you'll want to diagnose it - i.e. to lower the threshold and allow more verbose logging to understand what the process is actually doing (noting that this means more resources will be spent on logging - processor/disk/...).
_example: controlling verbosity with log levels_
```
LOG(Log::WRN) << "Some kind of bad state detected, you probably want this information in your logs for post-error diagnosis");
LOG(Log::DBG) << "A debug message, generally everything is fine. You may not want to spend resource logging this if everything's OK");
```
_possible LogIt log levels_
```cpp
    enum LOG_LEVEL
    {
        TRC = 0,
        DBG,
        INF,
        WRN,
        ERR
    };
```
The 6 million dollar question: which level is appropriate for which messages? LogIt leaves that decision to you.
## Components
Components is LogIt's way of allowing you, the user, to group related messages together. Each log message can be annotated with a (user-defined) component identifier, this has 2 main benefits:
1. In the log file, messages logged with the same component contain the same component-specific string: easier for searching.
2. The verbosity of each component can be controlled independently at runtime. Easier to log only relevant messages.

An example usage could be, say, using a component to log all invocations of some 3rd party API. At runtime, if there appears to be some problem with the functionality of whatever this API controls, then the verbosity of that component is increased, hopefully providing better insight into what the API calls are receiving and returning.
