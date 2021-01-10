cmake_minimum_required (VERSION 3.10)
project(dust)

include(cmake/pkg.cmake)

################################
# Library
################################
file(GLOB STORAGE_SRC_FILES src/storage/*.cc)
file(GLOB DOCUMENT_DB_SRC_FILES src/*.cc)
add_library(dust STATIC
  ${STORAGE_SRC_FILES}
  ${DOCUMENT_DB_SRC_FILES}
)
target_link_libraries(dust rapidjson boost boost-filesystem leveldb)
target_include_directories(dust PUBLIC include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(_definitions OS_MACOSX)
  set_target_properties(dust PROPERTIES COMPILE_DEFINITIONS "${_definitions}")
endif()
if(NOT MSVC)
  set_target_properties(dust PROPERTIES COMPILE_FLAGS "-std=c++11")
endif()


################################
# Unit Tests
################################
if (NOT TARGET gtest)
  add_subdirectory(test/gtest-1.7.0 EXCLUDE_FROM_ALL)
  enable_testing()
endif()
include_directories(SYSTEM ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB TEST_FILES test/*.cc)
add_executable(dust-tests EXCLUDE_FROM_ALL ${TEST_FILES})
target_link_libraries(dust-tests gtest gtest_main dust ${Boost_LIBRARIES})
set_target_properties(dust-tests PROPERTIES COMPILE_FLAGS "-std=c++11")
add_test(dust-tests dust-tests)
