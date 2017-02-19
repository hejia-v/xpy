find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )
set( Boost_USE_STATIC_LIBS        OFF )
set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_RUNTIME     OFF )
set( BOOST_ALL_DYN_LINK           ON )

include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${Python_LIBRARY_DIRS})  # 要在add_executable或add_library的前面

set(XPY_LIST
    ./xdef.h
    ./xpy.h
    ./xpy.cpp
    ./pybind_util_manual.h
    ./pybind_util_manual.cpp
)

source_group("" FILES ${XPY_LIST})

set(XPY_SHARED_TARGET "xpy")
add_library(${XPY_SHARED_TARGET} SHARED ${XPY_LIST})

target_link_libraries( ${XPY_SHARED_TARGET} ${Boost_LIBRARIES} )

set(XPY_SHARED_TARGET ${XPY_SHARED_TARGET} PARENT_SCOPE)
