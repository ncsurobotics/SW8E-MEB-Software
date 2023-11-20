cmake_minimum_required(VERSION 3.0)
project(SW8E-MEB-Firmware)


file(GLOB_RECURSE sources src/*.c include/*.h)

add_executable(firmware-build ${sources})

target_compile_options(firmware-build PUBLIC -Wall)

