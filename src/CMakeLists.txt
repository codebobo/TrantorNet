INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/log/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/net/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/thread/inc)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/tool/inc)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/log/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/net/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/thread/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/tool/src DIR_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/test DIR_SRC)

message(${PROJECT_SOURCE_DIR})
add_executable (trantor ${DIR_SRC})
