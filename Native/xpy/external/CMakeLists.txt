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

set(PYTHON_LIBRARIES_DEBUG_REQ
    python36_d.*
    _socket_d.*
    select_d.*
    pyexpat_d.*
)

set(PYTHON_LIBRARIES_DEBUG)

foreach(_comp ${PYTHON_LIBRARIES_DEBUG_REQ})
    file (GLOB CURRENT_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/PCbuild/amd64/${_comp})
    list(APPEND PYTHON_LIBRARIES_DEBUG ${CURRENT_LIBRARIES})
endforeach()

file (COPY ${PYTHON_LIBRARIES_DEBUG} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

if(COPY_PYTHON_LIB)
    file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Python-3.6.1/Lib DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../Assets/PyScript)
endif()

set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} PARENT_SCOPE)
set(Python_LIBRARY_DIRS ${Python_LIBRARY_DIRS} PARENT_SCOPE)

message("-- Python: 3.6.1")
# python end -----------------------------------------------------------------

add_subdirectory("fmt/" fmt)
set_property(TARGET fmt PROPERTY FOLDER "external")

# set(PHYSFS_ARCHIVE_ZIP OFF)
# set(PHYSFS_ARCHIVE_7Z OFF)
set(PHYSFS_ARCHIVE_GRP OFF)
set(PHYSFS_ARCHIVE_WAD OFF)
set(PHYSFS_ARCHIVE_HOG OFF)
set(PHYSFS_ARCHIVE_MVL OFF)
set(PHYSFS_ARCHIVE_QPAK OFF)
set(PHYSFS_ARCHIVE_SLB OFF)
set(PHYSFS_ARCHIVE_VDF OFF)
set(PHYSFS_ARCHIVE_ISO9660 OFF)
set(PHYSFS_BUILD_SHARED OFF)
add_subdirectory("physfs-3.0.1/" PhysicsFS)
set_property(TARGET physfs-static PROPERTY FOLDER "external")
set_property(TARGET test_physfs PROPERTY FOLDER "external")

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE"$ENV{CXXFLAGS} -O3 -Wall")
