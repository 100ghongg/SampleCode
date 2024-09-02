# SampleCode
ghong's Portfolio sample codes

1. loglib - ghong's logging library
 - google c++ coding style applied
 - add loglevel, time to logMsg
   "This is Logmsg!" -> "[Info] This is Logmsg! [2024-01-23 12:34:56.123]"
 - Setting detail log level adds Function name and File name, line number to logMsg
   "This is Logmsg!" -> "[Info] This is Logmsg! [foo()][bar.cpp:33][2024-01-23 12:34:56.123]"
 - Can write logs to stdout, file, db.
 - 2 Style Usage.
   1. Macro style
    LogUtils::DEBUG("This %s LogMsg!", "is");
   2. Class member func style
    glog::debug("This is LogMsg, %s", "too!");

2. libs
 - google c++ coding style applied
 - Singleton template
 - RingBuffer template
 - ThreadWrapper

3. game codes
 - Hungarian Notation coding style applied
 - IBB & OBB part of collision check code & rendering code
 - A* pathfind
