project(plantie_avr VERSION 0.0.0)

if (IS_WINDOWS)
    message("Running on Windows")
elseif(IS_UNIX)
    message("Running on LINUX")
endif()

## AVR Chip Configuration
# 20Mhz, this should match the crystal on your board,
set(F_CPU 20000000UL)
# CPU, you can find the list here:
# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html

# Select what Microcontroller the project uses
set(MCU atmega644)

# Default Baudrate for UART, read avr include/util/setbaud.h for usage
set(BAUD 9600)

# The programmer to use, read avrdude manual for list
set(PROGRAMMER usbasp)

# AVR Fuses, must be in concordance with your hardware and F_CPU
# http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p
set(E_FUSE 0xfd)
set(H_FUSE 0xda)
set(L_FUSE 0xfd)
set(LOCK_BIT 0xFF) # This means all lock bits are unprogrammed 

# set(CMAKE_C_FLAGS "-Wall -Os -mmcu=${MCU} ")
# set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS} "-fno-exceptions -fno-rtti")

# Pass defines to compiler
add_definitions(
    -DF_CPU=${F_CPU}
    -DBAUD=${BAUD}
)
# mmcu MUST be passed to both the compiler and linker, this handle the linker
set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

add_compile_options(
    -mmcu=${MCU} # MCU
    -std=gnu11 # C11 standard
    -Os # optimize
    -Wall # enable warnings
    -Wno-main
    -Wundef
    -pedantic
    -Wstrict-prototypes
    -Werror
    -Wfatal-errors
    -Wl,--relax,--gc-sections
    -g
    -gdwarf-2
    -funsigned-char # a few optimizations
    -funsigned-bitfields
    -fpack-struct
    -fshort-enums
    -ffunction-sections
    -fdata-sections
    -fno-split-wide-types
    -fno-tree-scev-cprop
)

# Automatically find all .cpp and .h files
file(GLOB_RECURSE C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message(STATUS "C files: ${C_FILES}")
message(STATUS "Header files: ${HEADER_FILES}")

# Create one target
add_executable(plantie_avr ${C_FILES} ${HEADER_FILES})

if (IS_WINDOWS)
    set_target_properties(plantie_avr PROPERTIES OUTPUT_NAME "plantie.elf")
elseif (IS_UNIX)
    set_target_properties(plantie_avr PROPERTIES OUTPUT_NAME "plantie.bin")
endif()

include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (IS_WINDOWS)
    message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${SYSTEMROOT}/avg-gcc/avr8-gnu-toolchain-win32_x86_64/avr/include
    )

    target_link_directories(${PROJECT_NAME} PUBLIC
        ${SYSTEMROOT}/avg-gcc/avr8-gnu-toolchain-win32_x86_64/avr/include
    )
    
elseif (IS_UNIX)
    target_include_directories(${PROJECT_NAME} PUBLIC
        /opt/
    )

    target_link_directories(${PROJECT_NAME} PUBLIC
        /opt/
    )
endif()

# build
#add_custom_target(build avr-gcc -mmcu=${MCU} -Wall -Os -I ${DEPENDENCY_FILES} -o ${PRODUCT_NAME}.elf ${SRC_FILES})

# Strip binary for upload
add_custom_target(strip ALL avr-strip ${CMAKE_BINARY_DIR}/bin/plantie.elf DEPENDS plantie_avr)

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${CMAKE_BINARY_DIR}/bin/plantie.elf ${CMAKE_BINARY_DIR}/bin/plantie.hex DEPENDS strip)

# Transform binary into eep file, this is the eeprom part (empty if you don't
# use eeprom static variables)
add_custom_target(eeprom ALL avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex ${CMAKE_BINARY_DIR}/bin/plantie.elf ${CMAKE_BINARY_DIR}/bin/plantie.eep DEPENDS strip)

# Adding make commands to be able to quickly programm the mcu
include(../config/cmake/avr_commands.cmake)

# Clean extra files
#set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")