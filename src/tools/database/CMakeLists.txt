
# Database library
add_library(database OBJECT 
    Database.cpp    
)
target_include_directories(database PUBLIC .)
target_link_libraries(database PUBLIC sqlite3)
