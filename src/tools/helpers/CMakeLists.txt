
# Helper library
add_library(helpers STATIC 
    IniFile.cpp
    String.cpp
    Timer.cpp
    TimerPool.cpp
    WorkerThreadPool.cpp
)
target_include_directories(helpers PUBLIC .)
