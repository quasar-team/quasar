<!---
/* © Copyright CERN, 2018.  All rights not expressly granted are reserved.
 * index.markdown
 *
 *  Created on: Jan 10, 2018
 * 		Author: Benjamin Farnham <benjamin.farnham@cern.ch>
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 */
--->
# LogIt
LogIt is designed to be a high performance, flexible, thread-safe logging solution for C++ projects

# Overall Architecture
## Front end
LogIt provides a logging front-end; this is the API your code will call to log messages via LogIt. The front-end is fixed, this is the API described in the interface file _LogIt.h_.

_example: logging a message with the LogIt API_
```cpp
// line below is line 10, in file main.cpp
LOG(Log::INF) << "hello, world!");
```
logs this output
```
2018-01-09 18:27.31.122088 [main.cpp:10, INF] hello, world!
```

## Back ends
LogIt delivers log messages to log repositories via back-ends. LogIt comes with some ready made back-end implementations (fairly standard stuff: stdout, a rolling file buffer based on the boost logging library, etc.) and an open interface for you to provide custom back-end implementations to deliver log messages to your custom log repositories.
## Verbosity
Key functionality in any logging library really, code using LogIt defines the messages that _can_ be logged, each message is annotated with a log level. At runtime, the logging threshold can be raised or lowered - the general idea being that raising the threshold means fewer messages are logged, lowering the threshold means more messages will be logged.
In a production system that is currently behaving well, the logging threshold would probably be left quite high (maybe just errors and warnings), thus keeping the resources spent on logging to a minimum. In a system exhibiting problems, you'll want to diagnose it - i.e. to lower the threshold and allow more verbose logging to understand what the process is actually doing (noting that this means more resources will be spent on logging - processor/disk/...).

_example: controlling verbosity with log levels_
```cpp
LOG(Log::WRN) << "Some kind of bad state detected, this information should probably always be logged");
LOG(Log::DBG) << "A debug message. You may not want to spend resource logging this if everything is working as expected");
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
The 6 million dollar question: which log level is appropriate for which messages? Should this message be a warning or just informational? LogIt leaves those difficult decisions to you. LogIt lets you adjust logging verbosity at runtime (for both the generic logger and for user-defined components). You tell LogIt the minimum logging level, messages below that level are ignored, messages at that level or above will be logged.

_example: setting the **generic** log level to TRC (i.e. log all messages), from user code_
```cpp
Log::setNonComponentLogLevel(Log::TRC);
```
_example: setting the log level for a **specific component** (see Components below) to DBG from user code (i.e. all messages for this component of level DBG or greater will be logged; TRC messages will be omitted)_
```cpp
Log::setComponentLogLevel(myComponentHandle, Log::DBG);
```

## Components
Components is LogIt's way of allowing you, the user, to group related messages together. Each log message can be annotated with a (user-defined) component identifier, this has 2 main benefits:
1. In the log file, messages logged with the same component contain the same component-specific string: easier for searching.
2. The verbosity of each component can be controlled independently at runtime. Easier to log only relevant messages.

An example usage could be, say, using a component to log all invocations of some 3rd party API. At runtime, if there appears to be some problem with the functionality of whatever this API controls, then the verbosity of that component can be increased, hopefully providing better insight into what the API calls are receiving and returning.

_example: registering a user-defined logging component_
```cpp
const Log::LogComponentHandle myHandle = Log::registerLoggingComponent("myComponentName", Log::INF);
```
Note: new logging components can be registered, but never deleted - this detail helps keep the cost of thread-safety for LogIt to reasonable levels. Once a user-defined component has been registered with LogIt there are 2 ways to log with it
1. **By handle:** Log using the unique handle returned from _registerLoggingComponent_

_example: logging with a component using its handle_
```cpp
LOG(Log::TRC, myHandle) << "message logged using component HANDLE";
```
2. **By name:** Log using the component name (note: name must be a case-sensitive match).

_example: logging with a component using the component name_
```cpp
LOG(Log::TRC, "myComponentName") << "message logged using component STRING";
```
Note that the methods above can be used interchangeably to log with the component, so, if, for some reason, you need to log with the component from an area of the code where the handle is not visible, you can log from there simply using the name. Whether a message is logged with the component handle or name, the resulting log message is the same.

_example: a log entry for a component_
```
2018-01-09 18:27.31.122088 [SomeFile.cpp:123, TRC, myComponentName] message logged using component ...
```
### Component Logging Performance Consideration: log with handle vs log with name
At runtime, LogIt must evaluate _every_ call to _LOG(...)_ to see whether its message should be sent to the backends (i.e. logged), or ignored. This decision is based on the relevant threshold for the component. LogIt tries to minimize the cost of these evaluations in line with the general C++ maxim (_don't pay for what you don't use_), however, an evaluation is required and therefore _LOG(...)_ invocations have an associated cost. What are the costs?
1. **log with handle == fast** : An internal implementation detail now - a component's handle actually identifies the position of an object managing that component's details held in an internal array. The component's details include the current verbosity setting. Evaluating _LOG(...)_ calls made with a component handle essentially require a simple access to that component object based on the handle then a simple, numerical, comparison. Cheap. 
2. **log with name == slower** : Another internal implementation detail - LogIt resolves calls to _LOG(...)_ made with component names to, essentially, the same call with the corresponding component handle. Clearly then, this requires an additional step: resolving the name to the handle. This is done using a look up in a map that LogIt maintains internally. Until some (as yet unknown) more efficient way of doing this is discovered, this is a more costly operation; _LOG(...)_ calls with component names require a thread-safe lookup in a map, and then (after successfully retrieving the appropriate handle) the same cost as a _LOG(...)_ invocation with a handle. Less cheap.

__Caveat: Implementations change faster than documentation - please resist any temptation to build splendiferous edifices in your code based on the information above, if the implementation changes those edifices may crumble. And I will cruelly point to this caveat. You can store handles returned from _register_ calls, and use those handles in _LOG(...)_ calls, or you can call _LOG(...)_ with component names. That will always be supported.__
