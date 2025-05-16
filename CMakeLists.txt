set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.24)

# specify cross compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(rplc C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

add_compile_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)


if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinOptDebInfo")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Og -g)
else()
    message(STATUS "No optimization")
endif()

include_directories(
    core/inc
    core/inc/f4
    thirdparty/stm32f4_hal/Inc
    thirdparty/stm32f4_hal/Inc/Legacy
    thirdparty/stm32f4_cmsis/Include
    thirdparty/cmsis/CMSIS/Core/Include
)

set(RHS_HAL_CAN ON)
# set(RHS_HAL_NETWORK ON)
set(RHS_HAL_RANDOM ON)
set(RHS_HAL_USB ON)
# set(RHS_SERVICE_LWIP ON)
add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx)

file(GLOB_RECURSE SOURCES "core/src/syscalls.c" "core/src/sysmem.c" "core/src/f4/*.*" "thirdparty/stm32f4_hal/*.*")
list(FILTER SOURCES EXCLUDE REGEX "_template[.]c$")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F407VETX_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

include(thirdparty/rhs/rhs.cmake)

add_executable(${PROJECT_NAME}.elf main.c ${SOURCES} ${LINKER_SCRIPT} "thirdparty/stm32f4_cmsis/Source/Templates/gcc/startup_stm32f407xx.s")
target_link_libraries(${PROJECT_NAME}.elf PUBLIC rhs rhs_hal)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

add_library(usb_core STATIC thirdparty/libusb_stm32/src/usbd_core.c thirdparty/libusb_stm32/src/usbd_stm32f429_otgfs.c)
target_compile_definitions(usb_core PUBLIC -DUSB_PMASIZE=512)
target_include_directories(usb_core
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libusb_stm32/inc>
)

add_subdirectory(thirdparty)
add_subdirectory(rlibs)
