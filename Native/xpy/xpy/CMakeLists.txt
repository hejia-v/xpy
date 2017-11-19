find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )
set( Boost_USE_STATIC_LIBS        OFF )
set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_RUNTIME     OFF )
set( BOOST_ALL_DYN_LINK           ON )

include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${Python_LIBRARY_DIRS})  # 要在add_executable或add_library的前面

include_directories("../external/fmt")
include_directories("../external/physfs-3.0.1/src")
include_directories("../external/rapidjson/include")
include_directories("./")

set(XPY_LIST
    ./xpyhelp.h
    ./xpyhelp.cpp
    ./sharppy.h
    ./sharppy.cpp
    ./xdefine.h
)

set(ALL_DIRS
    common
    helper
)

foreach(_comp ${ALL_DIRS})
    set(CURR_HEADER_LIST "")
    set(CURR_SRC_LIST "")
    set(SRC_DIR_NAME ./${_comp})
    aux_source_directory(${SRC_DIR_NAME} CURR_SRC_LIST)
    file (GLOB CURR_HEADER_LIST ${SRC_DIR_NAME}/*.h)
    list(APPEND XPY_LIST ${CURR_HEADER_LIST} ${CURR_SRC_LIST})
    source_group(${_comp} FILES ${CURR_HEADER_LIST} ${CURR_SRC_LIST})
endforeach()

source_group("" FILES ${XPY_LIST})

set(XPY_SHARED_TARGET "xpy")
add_library(${XPY_SHARED_TARGET} SHARED ${XPY_LIST})

target_link_libraries( ${XPY_SHARED_TARGET} ${Boost_LIBRARIES} )
target_link_libraries( ${XPY_SHARED_TARGET} "fmt" physfs-static)

set(XPY_SHARED_TARGET ${XPY_SHARED_TARGET} PARENT_SCOPE)
