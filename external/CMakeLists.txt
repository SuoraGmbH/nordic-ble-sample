add_library(
    nrf5_sdk STATIC
    nRF5_config/app_config.h
    "nRF5_SDK/components/ble/ble_advertising/ble_advertising.c"
    "nRF5_SDK/components/ble/common/ble_advdata.c"
    "nRF5_SDK/components/ble/common/ble_conn_params.c"
    "nRF5_SDK/components/ble/common/ble_conn_state.c"
    "nRF5_SDK/components/ble/common/ble_srv_common.c"
    "nRF5_SDK/components/ble/nrf_ble_gatt/nrf_ble_gatt.c"
    "nRF5_SDK/components/ble/peer_manager/gatt_cache_manager.c"
    "nRF5_SDK/components/ble/peer_manager/gatts_cache_manager.c"
    "nRF5_SDK/components/ble/peer_manager/id_manager.c"
    "nRF5_SDK/components/ble/peer_manager/peer_data_storage.c"
    "nRF5_SDK/components/ble/peer_manager/peer_database.c"
    "nRF5_SDK/components/ble/peer_manager/peer_id.c"
    "nRF5_SDK/components/ble/peer_manager/peer_manager.c"
    "nRF5_SDK/components/ble/peer_manager/pm_buffer.c"
    "nRF5_SDK/components/ble/peer_manager/security_dispatcher.c"
    "nRF5_SDK/components/ble/peer_manager/security_manager.c"
    "nRF5_SDK/components/boards/boards.c"
    "nRF5_SDK/components/libraries/atomic/nrf_atomic.c"
    "nRF5_SDK/components/libraries/atomic_fifo/nrf_atfifo.c"
    "nRF5_SDK/components/libraries/atomic_flags/nrf_atflags.c"
    "nRF5_SDK/components/libraries/balloc/nrf_balloc.c"
    "nRF5_SDK/components/libraries/bsp/bsp.c"
    "nRF5_SDK/components/libraries/button/app_button.c"
    "nRF5_SDK/components/libraries/crc16/crc16.c"
    "nRF5_SDK/components/libraries/crypto/backend/cc310/cc310_backend_ecc.c"
    "nRF5_SDK/components/libraries/crypto/backend/cc310/cc310_backend_init.c"
    "nRF5_SDK/components/libraries/crypto/backend/cc310/cc310_backend_mutex.c"
    "nRF5_SDK/components/libraries/crypto/backend/cc310/cc310_backend_rng.c"
    "nRF5_SDK/components/libraries/crypto/backend/cc310/cc310_backend_shared.c"
    "nRF5_SDK/components/libraries/crypto/nrf_crypto_ecc.c"
    "nRF5_SDK/components/libraries/crypto/nrf_crypto_init.c"
    "nRF5_SDK/components/libraries/crypto/nrf_crypto_rng.c"
    "nRF5_SDK/components/libraries/crypto/nrf_crypto_shared.c"
    "nRF5_SDK/components/libraries/log/src/nrf_log_backend_rtt.c"
    "nRF5_SDK/components/libraries/log/src/nrf_log_backend_serial.c"
    "nRF5_SDK/components/libraries/log/src/nrf_log_default_backends.c"
    "nRF5_SDK/components/libraries/log/src/nrf_log_frontend.c"
    "nRF5_SDK/components/libraries/log/src/nrf_log_str_formatter.c"
    "nRF5_SDK/components/libraries/experimental_section_vars/nrf_section_iter.c"
    "nRF5_SDK/components/libraries/memobj/nrf_memobj.c"
    "nRF5_SDK/components/libraries/fds/fds.c"
    "nRF5_SDK/components/libraries/fstorage/nrf_fstorage.c"
    "nRF5_SDK/components/libraries/fstorage/nrf_fstorage_sd.c"
    "nRF5_SDK/components/libraries/strerror/nrf_strerror.c"
    "nRF5_SDK/components/libraries/ringbuf/nrf_ringbuf.c"
    "nRF5_SDK/components/libraries/timer/app_timer.c"
    "nRF5_SDK/components/libraries/util/app_error.c"
    "nRF5_SDK/components/libraries/util/app_error_handler_gcc.c"
    "nRF5_SDK/components/libraries/util/app_error_weak.c"
    "nRF5_SDK/components/libraries/util/app_util_platform.c"
    "nRF5_SDK/components/libraries/util/sdk_mapped_flags.c"
    "nRF5_SDK/components/softdevice/common/nrf_sdh.c"
    "nRF5_SDK/components/softdevice/common/nrf_sdh_ble.c"
    "nRF5_SDK/components/softdevice/common/nrf_sdh_soc.c"
    "nRF5_SDK/external/fprintf/nrf_fprintf.c"
    "nRF5_SDK/external/fprintf/nrf_fprintf_format.c"
    "nRF5_SDK/external/segger_rtt/SEGGER_RTT.c"
    "nRF5_SDK/modules/nrfx/drivers/src/nrfx_gpiote.c"
    "nRF5_SDK/modules/nrfx/mdk/gcc_startup_${NRF_CHIP_NAME}.S"
    "nRF5_SDK/modules/nrfx/mdk/system_${NRF_CHIP_NAME}.c")

target_include_directories(
    nrf5_sdk PUBLIC
    nRF5_config
    "nRF5_SDK/config/${NRF_CHIP_NAME}/config"
    "nRF5_SDK/components/ble/ble_advertising"
    "nRF5_SDK/components/ble/common"
    "nRF5_SDK/components/ble/nrf_ble_gatt"
    "nRF5_SDK/components/ble/peer_manager"
    "nRF5_SDK/components/boards"
    "nRF5_SDK/components/device"
    "nRF5_SDK/components/libraries/atomic"
    "nRF5_SDK/components/libraries/atomic_fifo"
    "nRF5_SDK/components/libraries/atomic_flags"
    "nRF5_SDK/components/libraries/balloc"
    "nRF5_SDK/components/libraries/bsp"
    "nRF5_SDK/components/libraries/button"
    "nRF5_SDK/components/libraries/cli"
    "nRF5_SDK/components/libraries/crc16"
    "nRF5_SDK/components/libraries/crypto"
    "nRF5_SDK/components/libraries/crypto/backend/cc310"
    "nRF5_SDK/components/libraries/crypto/backend/cc310_bl"
    "nRF5_SDK/components/libraries/crypto/backend/cifra"
    "nRF5_SDK/components/libraries/crypto/backend/mbedtls"
    "nRF5_SDK/components/libraries/crypto/backend/micro_ecc"
    "nRF5_SDK/components/libraries/crypto/backend/nrf_hw"
    "nRF5_SDK/components/libraries/crypto/backend/nrf_sw"
    "nRF5_SDK/components/libraries/crypto/backend/oberon"
    "nRF5_SDK/components/libraries/crypto/backend/optiga"
    "nRF5_SDK/components/libraries/delay"
    "nRF5_SDK/components/libraries/log"
    "nRF5_SDK/components/libraries/log/src"
    "nRF5_SDK/components/libraries/memobj"
    "nRF5_SDK/components/libraries/fds"
    "nRF5_SDK/components/libraries/fstorage"
    "nRF5_SDK/components/libraries/mutex"
    "nRF5_SDK/components/libraries/queue"
    "nRF5_SDK/components/libraries/ringbuf"
    "nRF5_SDK/components/libraries/stack_info"
    "nRF5_SDK/components/libraries/strerror"
    "nRF5_SDK/components/libraries/timer"
    "nRF5_SDK/components/libraries/util"
    "nRF5_SDK/components/libraries/experimental_section_vars"
    "nRF5_SDK/components/softdevice/common"
    "nRF5_SDK/components/softdevice/common/softdevice_handler"
    "nRF5_SDK/components/softdevice/${NRF_SOFTDEVIDE}/headers"
    "nRF5_SDK/components/softdevice/${NRF_SOFTDEVIDE}/headers/nrf52"
    "nRF5_SDK/components/toolchain"
    "nRF5_SDK/components/toolchain/cmsis/include"
    "nRF5_SDK/external/fprintf"
    "nRF5_SDK/external/mbedtls/include"
    "nRF5_SDK/external/nrf_cc310/include"
    "nRF5_SDK/external/nrf_oberon"
    "nRF5_SDK/external/segger_rtt"
    "nRF5_SDK/integration/nrfx/legacy"
    "nRF5_SDK/modules/nrfx"
    "nRF5_SDK/modules/nrfx/drivers/include"
    "nRF5_SDK/modules/nrfx/hal"
    "nRF5_SDK/modules/nrfx/legacy"
    "nRF5_SDK/modules/nrfx/mdk"
    "nRF5_SDK/modules/nrfx/templates"
    "nRF5_SDK/modules/nrfx/templates/${NRFX_TEMPLATE_NAME}")

set_source_files_properties(
    "nRF5_SDK/components/boards/boards.c"
    PROPERTIES
    COMPILE_FLAGS "-include nrf_assert.h")

set_source_files_properties(
    "nRF5_SDK/components/libraries/button/app_button.c"
    PROPERTIES
    COMPILE_FLAGS "-include app_util_platform.h")

target_compile_definitions(
    nrf5_sdk
    PUBLIC "SOFTDEVICE_PRESENT" "USE_APP_CONFIG" "BOARD_PCA10056" "FLOAT_ABI_HARD" "NRF52840_XXAA" "S140" "NRF_SD_BLE_API_VERSION=6")

target_compile_options(
    nrf5_sdk
    INTERFACE "-Wno-register")

target_compile_options(
    nrf5_sdk
    PUBLIC "-Wno-unused-function" "-Wno-unused-variable")

target_link_libraries(
    nrf5_sdk PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/nRF5_SDK/external/nrf_cc310/lib/cortex-m4/hard-float/libnrf_cc310_0.9.12.a")
