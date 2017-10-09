# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(utar)

set(ver_maj 0)
set(ver_min 1)
set(ver_patch 0)
set(ver_string ${ver_maj}.${ver_min}.${ver_patch})

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(PROJECT_VERSION ${ver_string})

if (UNIX)
  if(APPLE)
    set(CMAKE_INSTALL_NAME_DIR "@rpath/")
  else()
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
  endif()
else()
  set(DLL_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif(UNIX)

set(CMAKE_CXX_STANDARD 11)

macro(msg m)
  message("## [utar] ${m}")
endmacro(msg)

msg("Got project source dir: ${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE export_hdr ${PROJECT_SOURCE_DIR}/export/*.h ${PROJECT_SOURCE_DIR}/*.hpp)

file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cpp)

msg("Got export hdrs: ${export_hdr}")
msg("Got src files: ${src_files}")

add_library(utar ${src_files})

if (BUILD_SHARED_LIBS)
  set(build_flag BUILDING_UTAR_SHARED)
else()
  set(build_flag UTAR_STATIC)
endif()

if (BUILD_SHARED_LIBS)
  set_target_properties(utar PROPERTIES VERSION ${ver_string}
                                      SOVERSION ${ver_maj})
endif()

target_compile_definitions(
  utar
  PRIVATE
    ${build_flag}
    UTAR_VER_MAJ=${ver_maj}
    UTAR_VER_MIN=${ver_min}
    UTAR_VER_PATCH=${ver_patch}
    UTAR_VER_STRING=${ver_string})

target_include_directories(
  utar
  PRIVATE include
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/export>
         $<INSTALL_INTERFACE:include>)

# 3P deps
set(3p ZLIB::zlib BZip2::bz2)

foreach(pkg ${3p})
  string(REPLACE ":" ";" _pkg_name ${pkg})
  list(GET _pkg_name 0 _pkg_name)
  find_package(${_pkg_name} CONFIG)
  msg("Finding ${_pkg_name}...")
  get_target_property(_loc ${pkg} LOCATION)
  msg("  ${pkg} -> ${_loc}")
  if (WIN32)
    if (BUILD_SHARED_LIBS)
      # get_target_property(_dll_dir utar LOCATION)
      # get_filename_component(_dll_dir ${_dll_dir} DIRECTORY)
      # msg("  utar dir: ${RUNTIME_OUTPUT_DIRECTORY}")
      file(COPY ${_loc} DESTINATION ${DLL_DIR})
    endif()
  endif()
endforeach()

target_link_libraries(utar PRIVATE ${3p})

#target_compile_features(
#  utar
#  PUBLIC
#  cxx_nullptr
#  cxx_lambdas
#  cxx_auto_type
#  cxx_static_assert)

configure_file(
  cmake/utarConfig.cmake.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/utarConfig.cmake @ONLY)

# INSTALL

export(
  TARGETS utar
  FILE ${PROJECT_BINARY_DIR}/utarTargets.cmake)

export(PACKAGE utar)

# install configs

install(
  TARGETS utar
  EXPORT utarTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

install(
  DIRECTORY export/
  DESTINATION include)

set(cmake_config_dir lib/cmake/utar)

install(
  FILES ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/utarConfig.cmake
  DESTINATION ${cmake_config_dir}
  COMPONENT dev)

install(
  EXPORT utarTargets
  DESTINATION ${cmake_config_dir}
  COMPONENT dev)

install(
  FILES ${PROJECT_SOURCE_DIR}/LICENSE
  DESTINATION share/utar
  RENAME LICENSE.utar)

# TESTS

include(tests.cmake)

# Packaging

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_VENDOR "Aaditya Kalsi")
set(CPACK_PACKAGE_CONTACT "akalsi.dev@gmail.com")
set(CPACK_STRIP_FILES 1)
include(CPack)
