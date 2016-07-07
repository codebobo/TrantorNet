INCLUDE_DIRECTORIES(/log/inc/)
INCLUDE_DIRECTORIES(/net/inc/)
INCLUDE_DIRECTORIES(/thread/inc/)
INCLUDE_DIRECTORIES(/tool/inc/)

aux_source_directory(/log/src/ LOG_SRC)
aux_source_directory(/net/src/ NET_SRC)
aux_source_directory(/thread/src/ THREAD_SRC)
aux_source_directory(/tool/src/ TOOL_SRC)
aux_source_directory(/test/ TEST_SRC)

add_executable (trantor ${LOG_SRC} ${NET_SRC} ${THREAD_SRC} ${TOOL_SRC} ${TEST_SRC})