cmake_minimum_required(VERSION 2.8)
project(prjece650)

if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found. Use `git clone https://github.com/agurfinkel/minisat` to check it out.")
endif()


## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)

find_package (Threads)

# create the main executable
## add additional .cpp files if needed
add_executable(ece650-prj ece650-prj.cpp)
#add_executable(nosat nosat.cpp)
#add_executable(project_base project_base.cpp)
#add_executable(thread thread.cpp)
# link MiniSAT libraries
target_link_libraries(ece650-prj minisat-lib-static)
target_link_libraries (ece650-prj ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(project_base minisat-lib-static)
#target_link_libraries (project_base ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(nosat minisat-lib-static)
#target_link_libraries (nosat ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(thread minisat-lib-static)
#target_link_libraries (thread ${CMAKE_THREAD_LIBS_INIT})

# create the executable for tests
#add_executable(test test.cpp)
# link MiniSAT libraries
#target_link_libraries(test minisat-lib-static)