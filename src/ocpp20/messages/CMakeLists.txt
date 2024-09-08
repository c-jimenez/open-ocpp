######################################################
#            OCPP 2.0 messages library               #
######################################################

# Library target
file (GLOB OCPP20_MESSAGES_SOURCE_FILES "*.cpp")
add_library(messages20 OBJECT
    ${OCPP20_MESSAGES_SOURCE_FILES}
)

# Exported includes
target_include_directories(messages20 PUBLIC .)

# Dependencies
target_link_libraries(messages20 PUBLIC types20 messages)
