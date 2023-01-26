
# Helper library
add_library(helpers OBJECT 
    IniFile.cpp
    StringHelpers.cpp
    Timer.cpp
    TimerPool.cpp
    WorkerThreadPool.cpp
)
target_include_directories(helpers PUBLIC .)
