if(CMAKE_HOST_WIN32)
    set(MSP430_GCC_PATH "C:/ti/msp430-gcc")
    set(EXE_EXTENSION ".exe")
elseif(CMAKE_HOST_APPLE)
    set(MSP430_GCC_PATH "/Applications/ti/msp430-gcc")
    set(EXE_EXTENSION "")
else()
    set(MSP430_GCC_PATH "/opt/ti/msp430-gcc")
    set(EXE_EXTENSION "")
endif()

# Allow overriding default paths with environment variables
if(DEFINED ENV{TI_MSP430_GCC_ROOT})
    set(MSP430_GCC_PATH "$ENV{TI_MSP430_GCC_ROOT}")
endif()

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          msp430)
set(CMAKE_CROSSCOMPILING 1)

set(TOOLCHAIN_PREFIX                ${MSP430_GCC_PATH}/bin/msp430-elf-)
set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc${EXE_EXTENSION} CACHE FILEPATH "C Compiler path")
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER} CACHE FILEPATH "ASM Compiler path")
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++${EXE_EXTENSION} CACHE FILEPATH "C++ Compiler path")
set(CMAKE_AR                        ${TOOLCHAIN_PREFIX}ar${EXE_EXTENSION} CACHE FILEPATH "AR path")
set(CMAKE_RANLIB                    ${TOOLCHAIN_PREFIX}ranlib${EXE_EXTENSION} CACHE FILEPATH "RANLIB path")
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy${EXE_EXTENSION} CACHE FILEPATH "OBJCOPY path")
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size${EXE_EXTENSION} CACHE FILEPATH "SIZE path")

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)