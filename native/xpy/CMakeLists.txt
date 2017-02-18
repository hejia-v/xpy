cmake_minimum_required(VERSION 3.6.0)

# CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS用来控制IF ELSE语句的书写方式
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

# Allow use of project folders for IDEs like Visual Studio, so we
# could organize projects into relevant folders: "cpr", "tests" & "external (libraries)".
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(xpy)

# Just debug / release since that's all that's all we need
set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if (WIN32)
    # This seems to break Xcode projects so definitely don't enable on Apple builds
    set(CMAKE_USE_RELATIVE_PATHS true)
    set(CMAKE_SUPPRESS_REGENERATION true)
endif()


if (MSVC)
    # Create debug libraries with _d postfix
    set(CMAKE_DEBUG_POSTFIX "_d")
endif()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

add_subdirectory("external/" external)
add_subdirectory("xpy/" xpy)
add_subdirectory("test/" test)

message("======================================>" ${PROJECT_BINARY_DIR})
