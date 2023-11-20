cmake_minimum_required(VERSION 3.20.0)
project(BlinkDemo)

# Do not set here! Define presets in CMakePresets.json!
string(TOLOWER "${TARGET_CHIP}" TARGET_CHIP_LOWER)
string(TOUPPER "${TARGET_CHIP}" TARGET_CHIP_UPPER)

################################################################################
# General Settings, flags, and paths
################################################################################
# Add additional source paths here
file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Add additional include paths here
set(INCLUDES
    "${MSP430_GCC_PATH}/include"
    "${PROJECT_SOURCE_DIR}/include"
)

# Add additional preprocessor definitions here
set(DEFINES
    __MSP430${TARGET_CHIP_UPPER}__
)

# Flags for both C compiler, C++ compiler, assembler, and linker
set(SHARED_FLAGS
    -mmcu=msp430${TARGET_CHIP_LOWER}
    -fno-exceptions
    -mhwmult=auto
)

# Flags only for C compiler (.c files)
set(CFLAGS
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
    -MMD
)

# Used only for assembler (.S files)
set(ASMFLAGS
    -x assembler-with-cpp
)

# Flags only for C++ compiler (.cc .cpp .cxx files)
set(CXXFLAGS
    -ffunction-sections
    -fdata-sections
    -fno-threadsafe-statics
    -MMD
    -fno-rtti
)

# Flags only for linker
set(LDFLAGS
    -L${MSP430_GCC_PATH}/include/
    -Tmsp430${TARGET_CHIP_LOWER}.ld
    -Wl,--gc-sections,-u,main
    -fno-rtti
)

# Target chip specific flags
if("${TARGET_CHIP_LOWER}" STREQUAL "g2553")
    set(SHARED_FLAGS ${SHARED_FLAGS} -msmall)
endif()

# C standard
set(REQ_C_STANDARD      11)
set(REQ_CPP_STANDARD    14)
################################################################################


################################################################################
# Firmware build
################################################################################
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_STANDARD ${REQ_C_STANDARD})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD ${REQ_CPP_STANDARD})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${INCLUDES})
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC ${DEFINES})
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC  $<$<COMPILE_LANGUAGE:ASM>:${SHARED_FLAGS} ${ASMFLAGS}>)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC  $<$<COMPILE_LANGUAGE:C>:${SHARED_FLAGS} ${CFLAGS}>)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC  $<$<COMPILE_LANGUAGE:CXX>:${SHARED_FLAGS} ${CXXFLAGS}>)
target_link_options(${CMAKE_PROJECT_NAME} PUBLIC ${SHARED_FLAGS} ${LDFLAGS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "firmware")
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}  POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex "$<OUTPUT_CONFIG:$<CONFIG>>/firmware.elf" "$<OUTPUT_CONFIG:$<CONFIG>>/firmware.hex"
)
################################################################################


################################################################################
# Flash target
################################################################################
if(CMAKE_HOST_WIN32)
    set(MSPDEBUG_BINARY "${PROJECT_SOURCE_DIR}/util/mspdebug.cmd")
else()
    set(MSPDEBUG_BINARY "${PROJECT_SOURCE_DIR}/util/mspdebug.sh")
endif()
add_custom_target(flash
    COMMAND ${MSPDEBUG_BINARY} --allow-fw-update tilib --force-reset "prog $<OUTPUT_CONFIG:$<CONFIG>>/firmware.hex"
    DEPENDS ${CMAKE_PROJECT_NAME}
    VERBATIM
    USES_TERMINAL
)
################################################################################


################################################################################
# Debug Server
################################################################################
if(CMAKE_HOST_WIN32)
    set(DEBUG_SERVER_HELPER "${PROJECT_SOURCE_DIR}/util/mspdebug_flash_gdb_helper.cmd")
else()
    set(DEBUG_SERVER_HELPER "${PROJECT_SOURCE_DIR}/util/mspdebug_flash_gdb_helper.sh")
endif()
add_custom_target(mspdebug-server
    COMMAND ${DEBUG_SERVER_HELPER} "$<OUTPUT_CONFIG:$<CONFIG>>/firmware.elf"
    DEPENDS ${CMAKE_PROJECT_NAME}
    VERBATIM
    USES_TERMINAL
)
################################################################################


################################################################################
# GDB launch
################################################################################
if(CMAKE_HOST_WIN32)
    set(GDB_BINARY "${PROJECT_SOURCE_DIR}/util/msp430-elf-gdb.cmd")
else()
    set(GDB_BINARY "${PROJECT_SOURCE_DIR}/util/msp430-elf-gdb.sh")
endif()
add_custom_target(gdb
    COMMAND ${GDB_BINARY} "$<OUTPUT_CONFIG:$<CONFIG>>/firmware.elf" -ex "target remote :2000"
    DEPENDS ${CMAKE_PROJECT_NAME}
    VERBATIM
    USES_TERMINAL
)
################################################################################