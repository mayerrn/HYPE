cmake_minimum_required(VERSION 3.2)

project(HYPE CXX C)

############################
###Compiler flags
############################
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17 -march=native -ferror-limit=2 -Wall")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fmax-errors=2 -O3 -march=native -Wall")
endif()

#build compilation database, needed for autocompletion in emacs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#link static
set(Boost_USE_STATIC_LIBS   ON)

# use boose program options
find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

# needed for multithreading
find_package (Threads)


#compile sources
add_executable(HYPE
    src/Hypergraph.cpp
    src/SSet.cpp
    src/Partition.cpp
    src/Partitioning.cpp
	src/Parsing.cpp
    main.cpp)

# make headers available
target_include_directories(HYPE PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIR})

#link against boost and thread libs
target_link_libraries(HYPE LINK_PUBLIC
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})
