# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.6.0)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )


set(BOOST_INCLUDE ${BOOST_DIR}/include)
set(BOOST_LIB ${BOOST_DIR}/libs/${ANDROID_ABI}/llvm-3.5)

include_directories(${BOOST_INCLUDE})
add_library(boost_system_lib STATIC IMPORTED)
add_library(boost_chrono_lib STATIC IMPORTED)
add_library(native_tst SHARED src/main/cpp/native_tst.cpp)
set_target_properties(boost_system_lib PROPERTIES IMPORTED_LOCATION ${BOOST_LIB}/libboost_system.a)
set_target_properties(boost_chrono_lib PROPERTIES IMPORTED_LOCATION ${BOOST_LIB}/libboost_chrono.a)
target_link_libraries(native_tst boost_chrono_lib boost_system_lib)

# ------------------------ xpy ------------------------
# CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS用来控制IF ELSE语句的书写方式
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

# ------------------------ xpy ------------------------
set(PYTHON_INCLUDE_DIRS ${PYTHON_DIR}/Include)
set(Python_LIBRARY_DIRS ${PYTHON_DIR})

set(XPY_DIR ../../Native/xpy)
add_subdirectory(${XPY_DIR}/external/fmt/ fmt)

include_directories(${PYTHON_DIR})
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${Python_LIBRARY_DIRS})  # 要在add_executable或add_library的前面

include_directories(${XPY_DIR}/external/fmt)
include_directories(${XPY_DIR}/external/rapidjson/include)
include_directories(${XPY_DIR}/xpy)

set(XPY_SRC_DIR ../../Native/xpy/xpy)

set(XPY_LIST
    ${XPY_SRC_DIR}/xpyhelp.h
    ${XPY_SRC_DIR}/xpyhelp.cpp
    ${XPY_SRC_DIR}/sharppy.h
    ${XPY_SRC_DIR}/sharppy.cpp
    ${XPY_SRC_DIR}/xdefine.h
)

set(ALL_DIRS
    common
    helper
)

foreach(_comp ${ALL_DIRS})
    set(CURR_HEADER_LIST "")
    set(CURR_SRC_LIST "")
    set(SRC_DIR_NAME ${XPY_SRC_DIR}/${_comp})
    aux_source_directory(${SRC_DIR_NAME} CURR_SRC_LIST)
    file (GLOB CURR_HEADER_LIST ${SRC_DIR_NAME}/*.h)
    list(APPEND XPY_LIST ${CURR_HEADER_LIST} ${CURR_SRC_LIST})
    source_group(${_comp} FILES ${CURR_HEADER_LIST} ${CURR_SRC_LIST})
endforeach()

source_group("" FILES ${XPY_LIST})

set(XPY_SHARED_TARGET "xpy")
add_library(${XPY_SHARED_TARGET} SHARED ${XPY_LIST})

add_library(boost_filesystem_lib STATIC IMPORTED)
set_target_properties(boost_filesystem_lib PROPERTIES IMPORTED_LOCATION ${BOOST_LIB}/libboost_filesystem.a)

add_library(python_lib SHARED IMPORTED)
set_target_properties(python_lib PROPERTIES IMPORTED_LOCATION ${PYTHON_DIR}/libpython3.7m.so)

target_link_libraries( ${XPY_SHARED_TARGET} boost_system_lib boost_filesystem_lib  ${log-lib} python_lib)
target_link_libraries( ${XPY_SHARED_TARGET} "fmt" )

# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -lldb")
set(CMAKE_CXX_FLAGS_RELEASE"$ENV{CXXFLAGS} -O3 -Wall")


message("======================================>")
message("======================================>" ${PROJECT_BINARY_DIR})
message(${BOOST_LIB})
message("======================================>")
# message(FATAL_ERROR "show log")  # 通过使用这句在android studio中抛出错误，从而显示log

# 查看导出函数
# D:\Android\sdk\ndk-bundle\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin\arm-linux-androideabi-nm.exe -D xx.so
# 查看依赖
# D:\Android\sdk\ndk-bundle\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin\arm-linux-androideabi-readelf.exe -d xx.so
