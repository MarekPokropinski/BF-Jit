cmake_minimum_required(VERSION 3.0)
project(BF_JIT)
set (CMAKE_CXX_STANDARD 11)
set(ASMJIT_PATH "C:/Programming/libs/asmjit" CACHE STRING "asmjit library location")

Include("${ASMJIT_PATH}/CMakeLists.txt")
# target_include_directories(asmjit PUBLIC
#     $<INSTALL_INTERFACE:${ASMJIT_PATH}/src>
#     $<BUILD_INTERFACE:${ASMJIT_PATH}/src>
# )

# file(GLOB asmjit_include_files CONFIGURE_DEPENDS
#     "${ASMJIT_PATH}/src/asmjit/*.h"
#     "${ASMJIT_PATH}/src/asmjit/*.cpp"
#     "${ASMJIT_PATH}/src/asmjit/core/*.cpp"
# )


# install(DIRECTORY ${ASMJIT_PATH}/src DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# add_library(asmjitlib ${asmjit_include_files})
# target_include_directories(asmjitlib PRIVATE ${ASMJIT_PATH})
# add_executable(asmjitlibexec ${asmjit_include_files})

include_directories(${ASMJIT_PATH}/src)

add_executable(Interpreter bf_interpreter.cpp)
add_executable(Jit bf_jit.cpp)
target_link_libraries(Jit PRIVATE asmjit)