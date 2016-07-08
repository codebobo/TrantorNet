INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/aio/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/net/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/thread/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/tool/inc)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/aio/src LOG_DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/test/logtest LOG_DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/tool/src LOG_DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/net/src LOG_DIR_SRC)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/aio/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/net/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/thread/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/tool/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/test/nettest DIR_SRC)

message(${PROJECT_SOURCE_DIR})
add_executable (trantor ${DIR_SRC})
add_executable (logtest ${LOG_DIR_SRC})

target_link_libraries(trantor pthread)
target_link_libraries(logtest pthread)

