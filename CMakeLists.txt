# Use the code in this section for standalone projects
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Gem)

# Find Garfield++
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

# Find ROOT
find_package(ROOT REQUIRED COMPONENTS Tree RIO Core)
include(${ROOT_USE_FILE})  # Sets up include and link dirs

# ---Build executable------------------------------------------------------------
add_executable(gem gem.cc)
target_link_libraries(gem Garfield::Garfield ${ROOT_LIBRARIES})

# --- Ensure the elmer directory exists in the build directory ---
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/elmer)

# ---Copy all necessary files locally to the build directory---
foreach(_file gem/gem.ep gem/gem.result gem/mesh.boundary gem/mesh.elements gem/mesh.header gem/mesh.nodes gem/dielectrics.dat)
  configure_file(${_file} ${CMAKE_CURRENT_BINARY_DIR}/elmer/ COPYONLY)
endforeach()

# --- Copy run.sh to build directory ---
configure_file(run.sh ${CMAKE_CURRENT_BINARY_DIR}/run.sh COPYONLY)

# Make run.sh executable
add_custom_command(
  TARGET gem POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E chmod +x ${CMAKE_CURRENT_BINARY_DIR}/run.sh
)