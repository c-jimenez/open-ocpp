
# Helper library
add_library(helpers OBJECT 
    IniFile.cpp
    String.cpp
    Timer.cpp
    TimerPool.cpp
    WorkerThreadPool.cpp
)
target_include_directories(helpers PUBLIC .)
