cmake_minimum_required(VERSION 3.20)

# Compile en mode Debug si -DCMAKE_BUILD_TYPE=Release non present
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Toolchain
set(TOOLCHAIN_PATH /opt/gcc-arm-none-eabi/bin)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-gcc -x assembler-with-cpp)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# Definition des varibles du projet
set(CMAKE_C_STANDARD_REQUIRED ON)
set(MCU "-mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_HAL_DRIVER -DSTM32F103xB -DUSE_FULL_ASSERT")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MCU}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g -ggdb -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s -DNDEBUG")

# Version
set(VERSION_NAME "TEST" CACHE INTERNAL "")
message(STATUS "VERSION_NAME ${VERSION_NAME}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVERSION_NAME='\"${VERSION_NAME}\"'")

# Install
set(INSTALL_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")


#######################
#        Projet       #
#######################
project(Relay C ASM)

# Creation de l'executable
add_executable(relay)

# Ajout des sources a la librarie
file(GLOB_RECURSE SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/hard/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug/*.c"
)

file(GLOB SRCS2
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_stm32f103xb.s)

target_sources(relay PRIVATE ${SRCS} ${SRCS2} ${STARTUP_SCRIPT})

target_include_directories(relay PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/hard
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/debug
)

target_link_libraries(relay PRIVATE
    ${MCU}
    --specs=nano.specs
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker/STM32F103C8Tx_FLASH.ld
    -lc
    -lm
    -lnosys
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

# Creation fichier .elf, .bin et .hex
set_target_properties(relay PROPERTIES SUFFIX ".elf")
add_custom_command(TARGET relay POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:relay> ${CMAKE_CURRENT_BINARY_DIR}/relay.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:relay> ${CMAKE_CURRENT_BINARY_DIR}/relay.bin
)

# Installation de l'executable dans le dossier d'installation
install(TARGETS relay DESTINATION ${INSTALL_DIRECTORY})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/relay.hex DESTINATION ${INSTALL_DIRECTORY})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/relay.bin DESTINATION ${INSTALL_DIRECTORY})


# Compile cmake and compile projet
# cmake . -B _build
# cmake --build _build --target install -j16
