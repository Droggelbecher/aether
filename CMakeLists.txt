
cmake_minimum_required(VERSION 3.1)

#set(CMAKE_CXX_FLAGS
	#"${CMAKE_CXX_FLAGS} -Wall"
#)
add_definitions("-Wall" "-g")

project(aether)

set(SRC_DIR src)
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cc)
add_executable(aether ${SOURCES})

# External Dependencies

find_package(SDL2 COMPONENTS gfx REQUIRED)
#find_package(SDL2_image REQUIRED)

find_package(fmt REQUIRED)
find_package(xtensor REQUIRED)
find_package(xtensor-blas REQUIRED)

include_directories(
	${SRC_DIR}
	${SDL2_INCLUDE_DIRS}
	#${SDL2_IMAGE_INCLUDE_DIRS}
)

set_property(TARGET aether PROPERTY CXX_STANDARD 17)

target_link_libraries(
	aether
	${SDL2_LIBRARIES}
	#${SDL2_IMAGE_LIBRARIES}
	SDL2_gfx
	SDL2_image
	fmt::fmt
	blas
)


