cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME horus_fw)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    "./Drivers/74HC595D/Src/74hc595d.c"
    "./Drivers/93C86/Src/93c86.c"
    "./Drivers/AHT20/Src/aht20.c"
    "./Drivers/HD44780/Src/hd44780.c"
    "./VexUF/Src/vexuf.c"
    "./VexUF/Src/vexuf_actuators.c"
    "./VexUF/Src/vexuf_adc.c"
    "./VexUF/Src/vexuf_avs.c"
    "./VexUF/Src/vexuf_buzzer.c"
    "./VexUF/Src/vexuf_cli.c"
    "./VexUF/Src/vexuf_config.c"
    "./VexUF/Src/vexuf_error.c"
    "./VexUF/Src/vexuf_i2c.c"
    "./VexUF/Src/vexuf_i2c_checker.c"
    "./VexUF/Src/vexuf_indicators.c"
    "./VexUF/Src/vexuf_lcd.c"
    "./VexUF/Src/vexuf_outputs.c"
    "./VexUF/Src/vexuf_pwm.c"
    "./VexUF/Src/vexuf_rtc.c"
    "./VexUF/Src/vexuf_sdcard.c"
    "./VexUF/Src/vexuf_serial.c"
    "./VexUF/Src/vexuf_spi.c"
    "./VexUF/Src/vexuf_tamper.c"
    "./VexUF/Src/vexuf_temperature.c"
    "./VexUF/Src/vexuf_timers.c"
    "./VexUF/Src/vexuf_tnc.c"
    "./VexUF/Src/vexuf_trigs.c"
    "./VexUF/Src/vexuf_ttl.c"
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "./Drivers/74HC595D/Inc"
    "./Drivers/93C86/Inc"
    "./Drivers/AHT20/Inc"
    "./Drivers/HD44780/Inc"
    "./VexUF/Inc"
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Add the floating-point support for printf
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float")


# Generate HEX and BIN files and copy them if the build type is Release or MinSizeRel
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    # Define the output directory
    set(OUTPUT_DIR "${PROJECT_SOURCE_DIR}/binaries/${CMAKE_BUILD_TYPE}")

    # Create the output directory if it doesn't exist
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    # Convert ELF to HEX
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
        COMMENT "Building ${CMAKE_PROJECT_NAME}.hex"
    )

    # Convert ELF to BIN
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
        COMMENT "Building ${CMAKE_PROJECT_NAME}.bin"
    )

    # Copy the files to the output directory
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${OUTPUT_DIR}/${CMAKE_PROJECT_NAME}.elf
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.hex ${OUTPUT_DIR}/${CMAKE_PROJECT_NAME}.hex
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.bin ${OUTPUT_DIR}/${CMAKE_PROJECT_NAME}.bin
        COMMENT "Copying files to ${OUTPUT_DIR}"
    )
endif()