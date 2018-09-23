cmake_minimum_required (VERSION 2.8)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
***************************************************************************
***      In place build is a bad idea. Please build out-of-place.       ***
***   Run cmake /path/to/source from a directory != /path/to/source.    ***
***                                                                     ***
*** DON'T FORGET TO REMOVE THE CMakeCache.txt IN YOUR SOURCE DIRECTORY  ***
***                  THAT WAS JUST CREATED.                             ***
***************************************************************************
")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


project (CellSim)
message(STATUS "Configuring project \"${PROJECT_NAME}\"")
message(STATUS "+    project source dir: ${PROJECT_SOURCE_DIR}")

set(CELLSIM_CONFIGURED_SUCCESSFULLY TRUE
    CACHE BOOL "${PROJECT_NAME} configured successfully." FORCE)

#include(${CMAKE_SOURCE_DIR}/cmake/config.cmake)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CACHE PATH "Output directory for shared libraries." FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CACHE PATH "Output directory for executables like tests.")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/tools/)
find_package (Threads REQUIRED)

find_package(SDL2 REQUIRED)
#find_package(SDL2_ttf REQUIRED)

find_package(OpenGL REQUIRED)
FIND_PACKAGE(Boost REQUIRED)
    IF (Boost_FOUND)
    message(STATUS "+-- boost found")
        INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
        ADD_DEFINITIONS( "-DHAS_BOOST" )
    ENDIF()


include_directories(
    include
    ${SDL2_INCLUDE_DIR}
)

add_executable(CellSim src/CellSim.cxx src/CellApp.cxx src/CellSimulation.cxx)
add_executable(CellSim2 src/CellMain.cxx src/CellSimulation2.cxx src/Graph.cxx src/FontCache.cxx)

add_executable(Graph src/GraphMain.cxx src/FontCache.cxx src/Graph.cxx)

add_executable(Graph2 src/GraphMain2.cxx src/FontCache.cxx src/Graph.cxx)

add_executable(Particles src/ParticleMain.cxx 
                        src/ParticleSimulation.cxx 
                        src/FontCache.cxx 
                        src/Graph.cxx 
                        src/QuadTree.cxx
                        src/colormap.cxx)

add_executable(ParticlesNoGui src/ParticlesNoGui.cxx src/ParticleSimulation.cxx src/QuadTree.cxx)

target_link_libraries(CellSim SDL2)
target_link_libraries(CellSim2 SDL2 SDL2_ttf)

target_link_libraries(Graph SDL2 SDL2_ttf)
target_link_libraries(Graph2 SDL2 SDL2_ttf)
target_link_libraries(Particles SDL2 SDL2_ttf )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -pthread -std=c++14")
add_definitions( -O3 -g -Wall -Wextra)