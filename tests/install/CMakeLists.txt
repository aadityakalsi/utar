# CMakeLists.txt for install

cmake_minimum_required(VERSION 3.0)

set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})

file(GLOB src_files ${TESTS_DIR}/*.c ${TESTS_DIR}/*.cpp)
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR}/_source)
file(COPY ${src_files} DESTINATION ${src_dir})

find_package(utar)

file(GLOB src_files ${src_dir}/*.c ${src_dir}/*.cpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(tests_install ${src_files})

target_include_directories(
  tests_install
  PRIVATE ${TESTS_DIR})

target_link_libraries(tests_install utar)

if (WIN32)
  set(cmd_env ${CMAKE_COMMAND} -E env PATH="\$PATH$<SEMICOLON>${CMAKE_INSTALL_PREFIX}/bin" $<TARGET_FILE:tests_install>)
else()
  set(cmd_env $<TARGET_FILE:tests_install>)
endif()

add_custom_target(
  tests_run
  COMMAND ${cmd_env}
  DEPENDS tests_install)
