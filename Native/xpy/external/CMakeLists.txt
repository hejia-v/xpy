# python start ---------------------------------------------------------------
# set(CMAKE_VERBOSE_MAKEFILE on)
if (WIN32)
    set(PYTHON_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/PC
    )
    set(Python_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/PCbuild/amd64)
else()
    set(PYTHON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/Include)
    set(Python_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/PCbuild/amd64)
endif()

file (GLOB PYTHON_LIBRARIES_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/PCbuild/amd64/python36_d.*)
file (COPY ${PYTHON_LIBRARIES_DEBUG} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} PARENT_SCOPE)
set(Python_LIBRARY_DIRS ${Python_LIBRARY_DIRS} PARENT_SCOPE)

message("-- Python: 3.6.1")
# python end -----------------------------------------------------------------

add_subdirectory("fmt/" fmt)
set_property(TARGET fmt PROPERTY FOLDER "external")

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE"$ENV{CXXFLAGS} -O3 -Wall")
