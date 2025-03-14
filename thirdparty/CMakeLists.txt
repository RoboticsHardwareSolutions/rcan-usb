####################### FREERTOS lib ############################
add_library(freertos_config INTERFACE)

target_include_directories(freertos_config SYSTEM
        INTERFACE
        ../Core/Inc
)

target_compile_definitions(freertos_config
        INTERFACE
        projCOVERAGE_TEST=0
)

add_subdirectory(freertos)

####################### RTT lib ############################
add_library(RTT STATIC
        RTT/RTT/SEGGER_RTT_ASM_ARMv7M.S
        RTT/RTT/SEGGER_RTT_printf.c
        RTT/RTT/SEGGER_RTT.c
)

target_compile_definitions(RTT PUBLIC -DBUFFER_SIZE_DOWN=64)

target_include_directories(RTT
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/RTT/RTT>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/RTT/Config>
)

add_subdirectory(rhs)
