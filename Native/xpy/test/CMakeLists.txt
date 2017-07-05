# SET(ENV{BOOST_ROOT} "/usr/lib")
find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )
set( Boost_USE_STATIC_LIBS        OFF )
set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_RUNTIME     OFF )
set( BOOST_ALL_DYN_LINK           ON )

include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${Python_LIBRARY_DIRS})  # 要在add_executable或add_library的前面

set(TEST_LIST
    ./main.cpp
)
add_executable(testmain ${TEST_LIST})
source_group("" FILES ${TEST_LIST})

include_directories(../xpy)
add_dependencies(testmain ${XPY_SHARED_TARGET})
target_link_libraries(testmain ${XPY_SHARED_TARGET})
target_link_libraries(testmain ${Boost_LIBRARIES})

# add_definitions("/wd4251 /wd4221 /wd4800")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testmain)
set(CMAKE_DEFAULT_STARTUP_PROJECT testmain)
