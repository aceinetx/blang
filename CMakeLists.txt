cmake_minimum_required(VERSION 3.20)
project(blang)

find_package(LLVM REQUIRED CONFIG)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(tl-expected REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(LINUX)
	add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-fsanitize=address")
	add_link_options("-fsanitize=address")
endif()

add_definitions(${LLVM_DEFINITIONS})

bison_target(Parser src/bison/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cc DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh)
flex_target(Lexer src/bison/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)
add_flex_bison_dependency(Lexer Parser)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

include_directories(${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_BINARY_DIR} src)

add_executable(blang ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
target_link_libraries(blang LLVM fmt::fmt-header-only)
target_precompile_headers(blang PRIVATE "pch.hh")
