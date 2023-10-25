cmake_minimum_required (VERSION 2.8)

project (demo)

set (TAGET demo)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# aux_source_directory (./ TOP_SRC)
# aux_source_directory (src SUB_SRC)

# set (SRC_LIST 
# 	${TOP_SRC} 
# 	${SUB_SRC}
# 	)

aux_source_directory (./ TOP_SRC)
aux_source_directory (src SUB_SRC)

set (SRC_LIST 
	${PROJECT_SOURCE_DIR}/src/mycan.cpp
	${PROJECT_SOURCE_DIR}/main.cpp
	)


include_directories (${PROJECT_SOURCE_DIR}
					 inc 
					 )

add_executable(${TAGET}
			${SRC_LIST}
			)

target_link_libraries(${TAGET}
		)
