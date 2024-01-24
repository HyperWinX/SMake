#Project setup
cmake_minimum_required(VERSION 3.5)
project(SMake CXX)

#Check packages
find_program(CLANG_EXECUTABLE clang)
if(CLANG_EXECUTABLE)
    execute_process(COMMAND ${CLANG_EXECUTABLE} -v
                    RESULT_VARIABLE EXITCODE
                    ERROR_VARIABLE CLANG_VERSION_OUTPUT)
    if (EXITCODE EQUAL 0)
        string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CLANG_VERSION ${CLANG_VERSION_OUTPUT})
        message("CLang version: ${CLANG_VERSION}")
    else()
        message(FATAL_ERROR "Failed to get CLang version")
    endif()
else()
    message(FATAL_ERROR "CLang is not installed")
endif()

#Set up executables
add_executable(smake main.cpp parser.cpp fileops.cpp)
set(CMAKE_CXX_FLAGS "-g -O0")
#add_custom_command(TARGET smake POST_BUILD COMMAND gdb smake)