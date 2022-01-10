
# JSON tools library is an interface wrapper for the rapidjson
# library which disable the warnings coming from the rapidjson's headers
# and provides some helper classes
add_library(json STATIC JsonValidator.cpp)
target_include_directories(json PUBLIC .)
target_link_libraries(json rapidjson)
