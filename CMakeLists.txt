cmake_minimum_required(VERSION 3.20)
project(blang)

find_package(LLVM REQUIRED CONFIG)
find_package(BISON REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(LINUX)
        add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-fsanitize=address")
        add_link_options("-fsanitize=address")
endif()

add_definitions(${LLVM_DEFINITIONS})

bison_target(Parser src/frontend/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cc")

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

include_directories(${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} src)

add_executable(blang ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
target_link_libraries(blang LLVM fmt::fmt)
