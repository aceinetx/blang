cmake_minimum_required(VERSION 3.20)
project(blang)

find_package(LLVM REQUIRED CONFIG)
find_package(fmt CONFIG REQUIRED)

if(LINUX)
	add_compile_options("-Wall" "-Wextra" "-Wpedantic")
endif()
add_definitions(${LLVM_DEFINITIONS})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

include_directories(${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(blang ${SOURCES})
target_link_libraries(blang LLVM fmt::fmt-header-only)
target_precompile_headers(blang PRIVATE "pch.hh")
