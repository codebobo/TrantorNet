INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/aio/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/net/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/thread/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/tool/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/tcp/inc)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/aio/src LOG_DIR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/tool/src LOG_DIR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/test/logtest LOG_DIR_SRC)
#aux_source_directory(${PROJECT_SOURCE_DIR}/src/tool/src LOG_DIR_SRC)
#aux_source_directory(${PROJECT_SOURCE_DIR}/src/net/src LOG_DIR_SRC)

#aux_source_directory(${PROJECT_SOURCE_DIR}/src/aio/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/aio/src TRANTOR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/tool/src TRANTOR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/net/src TRANTOR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/thread/src TRANTOR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/tool/src TRANTOR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/tcp/src TRANTOR_LIB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/test/nettest DIR_SRC)

add_library(logplus ${LOG_DIR_LIB})
add_library(trantor ${TRANTOR_LIB})

message(${PROJECT_SOURCE_DIR})
add_executable (nettest ${DIR_SRC})
add_executable (logtest ${LOG_DIR_SRC})

target_link_libraries(nettest trantor pthread libuuid.so)
target_link_libraries(logtest logplus pthread)

