
cmake_minimum_required(VERSION 3.1)

#set(CMAKE_CXX_FLAGS
	#"${CMAKE_CXX_FLAGS} -Wall"
#)
add_definitions("-Wall" "-g")

project(aether)

set(SRC_DIR src)
file(GLOB SOURCES ${SRC_DIR}/*.cc)
add_executable(aether ${SOURCES})

# External Dependencies

find_package(SDL2 COMPONENTS gfx REQUIRED)
find_package(fmt REQUIRED)
find_package(xtensor REQUIRED)

include_directories(
	${SRC_DIR}
	${SDL2_INCLUDE_DIRS}
)

target_link_libraries(
	aether
	${SDL2_LIBRARIES}
	fmt::fmt
	SDL2_gfx
)


