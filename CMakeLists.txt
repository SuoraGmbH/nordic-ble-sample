cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(nordic-ble-sample VERSION 1.0.0 LANGUAGES C CXX ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CMakePrintHelpers)

set(NRF_CHIP_NAME nrf52840)
set(NRFX_TEMPLATE_NAME nRF52840)
set(NRF_SOFTDEVIDE s140)

include(buildType)
include(compileFlags)
include(buildInformation)

find_program(NRFJPROG nrfjprog)

link_directories("external/nRF5_SDK/modules/nrfx/mdk")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/external/nRF5_SDK/config/${NRF_CHIP_NAME}/armgcc/generic_gcc_nrf52.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT}")

function(register_firmware_target target_name)
    set_target_properties("${target_name}" PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")

    add_custom_command(
        OUTPUT "${target_name}.hex"
        COMMAND "${CMAKE_OBJCOPY}" "-O" "ihex" "$<TARGET_FILE:${target_name}>" "${target_name}.hex"
        DEPENDS "${target_name}"
        COMMENT "Generate IHEX for executable ${target_name}")

    add_custom_target(
        "${target_name}-hex-files" ALL
        DEPENDS "${target_name}.hex")

    if(NRFJPROG)
        add_custom_target(
            "flash-${target_name}-full"
            DEPENDS "${target_name}.hex"
            COMMENT "flash device"
            COMMAND "${NRFJPROG}" "-f" "nrf52" "--program" "${CMAKE_SOURCE_DIR}/external/${NRF_SOFTDEVIDE}/${NRF_SOFTDEVIDE}_nrf52_7.0.1_softdevice.hex" "--sectorerase"
            COMMAND "${NRFJPROG}" "-f" "nrf52" "--program" "${target_name}.hex" "--sectorerase"
            COMMAND "${NRFJPROG}" "-f" "nrf52" "--reset"
            USES_TERMINAL)

        add_custom_target(
            "flash-${target_name}"
            DEPENDS "${target_name}.hex"
            COMMENT "flash device"
            COMMAND "${NRFJPROG}" "-f" "nrf52" "--program" "${target_name}.hex" "--sectorerase"
            COMMAND "${NRFJPROG}" "-f" "nrf52" "--reset"
            USES_TERMINAL)
    endif()
endfunction()

add_subdirectory(external)
add_subdirectory(src)
