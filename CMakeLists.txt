cmake_minimum_required (VERSION 2.6)
project(dust)


################################
# External libraries
################################
add_subdirectory(external_lib/leveldb)
include_directories("external_lib/leveldb/include")
include_directories("external_lib/rapidjson/include")


################################
# Includes
################################
include_directories("include")


################################
# Boost
################################
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS
  system
  filesystem
REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
	message("No Boost found.")
endif(Boost_FOUND)
link_directories(${CMAKE_LIBRARY_PATH})


################################
# Library
################################
file(GLOB STORAGE_SRC_FILES src/storage/*.cc)
file(GLOB DOCUMENT_DB_SRC_FILES src/*.cc)
add_library(dust STATIC
  ${STORAGE_SRC_FILES}
  ${DOCUMENT_DB_SRC_FILES}
)
target_link_libraries(dust leveldb)

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
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB TEST_FILES test/*.cc)
add_executable(dust-tests EXCLUDE_FROM_ALL ${TEST_FILES})
target_link_libraries(dust-tests gtest gtest_main dust ${Boost_LIBRARIES})
set_target_properties(dust-tests PROPERTIES COMPILE_FLAGS "-std=c++11")
add_test(dust-tests dust-tests)
