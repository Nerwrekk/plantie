cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# toolchain starts with defining mandatory variables
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)

if (WIN32)
    set(IS_WINDOWS TRUE CACHE INTERNAL "Running on Windows")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(UNIX)
    set(IS_UNIX TRUE CACHE INTERNAL "Running on Unix")
endif()

if (WIN32)
elseif (UNIX)
endif()

set(CMAKE_VERBOSE_MAKEFILE true)

if (WIN32)
    #This function will try and look for the file you specified by giving it a starting point
    #It will look for the file in all subdirectories and multiple paths can be added if it does not find it
    find_path(C_COMPILER_PATH avr-gcc.exe
            PATHS ${SYSTEMROOT}/avr-gcc/
            ) #Note that this does not include the file name! just the path to it!

    find_path(CXX_COMPILER_PATH avr-g++.exe
            PATHS ${SYSTEMROOT}/avr-gcc/
            )

    if(C_COMPILER_PATH)
            message("Found avr-gcc.exe file in ${C_COMPILER_PATH}")
    endif()
elseif(UNIX)
endif()

#Theese must always be set before project() is called
# C:/avr-gcc/avr8-gnu-toolchain-win32_x86_64/bin/avr-gcc.exe
if (WIN32)
    SET(CMAKE_C_COMPILER    "${C_COMPILER_PATH}/avr-gcc.exe")
    SET(CMAKE_CXX_COMPILER  "${CXX_COMPILER_PATH}/avr-g++.exe")
    set(CMAKE_ASM_COMPILER  "${C_COMPILER_PATH}/avr-gcc.exe") #ASM uses the same compiler as for C compiler 
elseif(UNIX)
    SET(CMAKE_C_COMPILER    ${C_COMPILER_PATH}/${C_COMPILER})
    SET(CMAKE_CXX_COMPILER  ${CXX_COMPILER_PATH}/${CXX_COMPILER})
    SET(CMAKE_ASM_COMPILER  ${C_COMPILER_PATH}/${C_COMPILER}) #ASM uses the same compiler as for C compiler 
endif()

# Theese two are a must because we know it will work but cmakes simple test will fail
SET (CMAKE_C_COMPILER_WORKS true) 
SET (CMAKE_CXX_COMPILER_WORKS true)

project(plantie)
set(MCU atmega644)

# Set the C++ standard
set(CMAKE_CONFIGURATION_TYPES Debug Release) 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE) 
set(CMAKE_C_EXTENSIONS ON) 

# build output directories 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_PDB_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib) 
set(EXECUTABLE_OUTPUT_PATH $<0:>${CMAKE_BINARY_DIR}/bin)

# project subdirectories 
add_subdirectory(plantie_avr) 


