
# Database library
add_library(database STATIC 
    Database.cpp    
)
target_include_directories(database PUBLIC .)
target_link_libraries(database sqlite3)
