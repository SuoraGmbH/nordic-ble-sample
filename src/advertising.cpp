#include <app_timer.h>
#include <ble_advertising.h>
#include <ble_conn_params.h>
#include <bsp.h>
#include <nrf_ble_gatt.h>
#include <nrf_delay.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>

#include "array.hpp"

using namespace suora::ble;

namespace {

NRF_BLE_GATT_DEF(bleGattInstance);
BLE_ADVERTISING_DEF(bleAdvertisingInstance);

void abort_on_error(ret_code_t error_code) {
    APP_ERROR_HANDLER(error_code);
}

void advertising_event_handler(ble_adv_evt_t event) {
    ret_code_t error_code;

    switch (event) {
    case BLE_ADV_EVT_FAST:
        error_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
        APP_ERROR_CHECK(error_code);
        break;
    case BLE_ADV_EVT_SLOW:
        error_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW);
        APP_ERROR_CHECK(error_code);
        break;
    case BLE_ADV_EVT_IDLE:
        error_code = bsp_indication_set(BSP_INDICATE_IDLE);
        APP_ERROR_CHECK(error_code);
        break;
    default:
        break;
    }
}
}

int main() {
    ret_code_t error_code;

    error_code = NRF_LOG_INIT(nullptr);
    APP_ERROR_CHECK(error_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    error_code = app_timer_init();
    APP_ERROR_CHECK(error_code);

    error_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(error_code);

    while (!nrf_sdh_is_enabled()) {
        if (!NRF_LOG_PROCESS()) {
            nrf_delay_ms(50);
        }
    }

    error_code = bsp_init(BSP_INIT_LEDS, nullptr);
    APP_ERROR_CHECK(error_code);

    uint32_t application_ram_start_address;
    error_code = nrf_sdh_ble_default_cfg_set(1, &application_ram_start_address);
    APP_ERROR_CHECK(error_code);

    error_code = nrf_sdh_ble_enable(&application_ram_start_address);
    APP_ERROR_CHECK(error_code);

    ble_gap_conn_sec_mode_t security_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&security_mode);
    auto device_name = to_array_without_null("BLE Sample 01");
    error_code = sd_ble_gap_device_name_set(&security_mode, device_name.data(), device_name.size());
    APP_ERROR_CHECK(error_code);

    error_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_WATCH);
    APP_ERROR_CHECK(error_code);

    ble_gap_conn_params_t gapConnectionParameters;
    memset(&gapConnectionParameters, 0, sizeof(gapConnectionParameters));
    gapConnectionParameters.conn_sup_timeout = MSEC_TO_UNITS(4000, UNIT_10_MS);
    gapConnectionParameters.max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS);
    gapConnectionParameters.min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS);
    gapConnectionParameters.slave_latency = 0;

    error_code = sd_ble_gap_ppcp_set(&gapConnectionParameters);
    APP_ERROR_CHECK(error_code);

    error_code = nrf_ble_gatt_init(&bleGattInstance, nullptr);
    APP_ERROR_CHECK(error_code);

    ble_advertising_init_t advertisingInitData;
    memset(&advertisingInitData, 0, sizeof(advertisingInitData));

    advertisingInitData.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advertisingInitData.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advertisingInitData.advdata.include_ble_device_addr = true;
    advertisingInitData.evt_handler = &advertising_event_handler;
    advertisingInitData.error_handler = &abort_on_error;
    advertisingInitData.config.ble_adv_fast_enabled = true;
    advertisingInitData.config.ble_adv_fast_timeout = 60;
    advertisingInitData.config.ble_adv_fast_interval = MSEC_TO_UNITS(200u, UNIT_0_625_MS);
    advertisingInitData.config.ble_adv_slow_enabled = true;
    advertisingInitData.config.ble_adv_slow_timeout = 0;
    advertisingInitData.config.ble_adv_slow_interval = MSEC_TO_UNITS(500u, UNIT_0_625_MS);

    error_code = ble_advertising_init(&bleAdvertisingInstance, &advertisingInitData);
    APP_ERROR_CHECK(error_code);

    ble_advertising_conn_cfg_tag_set(&bleAdvertisingInstance, 1);

    error_code = ble_advertising_start(&bleAdvertisingInstance, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(error_code);

    NRF_LOG_INFO("Starting the main loop");
    while (true) {
        if (!NRF_LOG_PROCESS()) {
            error_code = sd_app_evt_wait();
            APP_ERROR_CHECK(error_code);
        }
    }
}
