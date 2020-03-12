#include <app_timer.h>
#include <ble_advertising.h>
#include <ble_conn_params.h>
#include <bsp.h>
#include <fds.h>
#include <nrf_ble_gatt.h>
#include <nrf_delay.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <peer_manager.h>

#include "array.hpp"

using namespace suora::ble;

namespace {

NRF_BLE_GATT_DEF(gatt_instance);
BLE_ADVERTISING_DEF(advertising_instance);

void error_handler(ret_code_t error_code) {
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

void peer_manager_event_handler(const pm_evt_t *event) {
    ret_code_t errorCode;

    switch (event->evt_id) {
    case PM_EVT_STORAGE_FULL:
        errorCode = fds_gc();
        if (errorCode != FDS_ERR_BUSY && errorCode != FDS_ERR_NO_SPACE_IN_QUEUES) {
            APP_ERROR_CHECK(errorCode);
        }
        break;

    case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        pm_local_database_has_changed();
        break;

    case PM_EVT_PEER_DATA_UPDATE_FAILED:
        APP_ERROR_HANDLER(event->params.peer_data_update_failed.error);
        break;

    case PM_EVT_PEER_DELETE_FAILED:
        APP_ERROR_HANDLER(event->params.peer_delete_failed.error);
        break;

    case PM_EVT_PEERS_DELETE_FAILED:
        APP_ERROR_HANDLER(event->params.peers_delete_failed_evt.error);
        break;

    case PM_EVT_ERROR_UNEXPECTED:
        APP_ERROR_HANDLER(event->params.error_unexpected.error);
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

    ble_gap_conn_params_t gap_connection_parameters;
    memset(&gap_connection_parameters, 0, sizeof(gap_connection_parameters));
    gap_connection_parameters.conn_sup_timeout = MSEC_TO_UNITS(4000, UNIT_10_MS);
    gap_connection_parameters.max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS);
    gap_connection_parameters.min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS);
    gap_connection_parameters.slave_latency = 0;

    error_code = sd_ble_gap_ppcp_set(&gap_connection_parameters);
    APP_ERROR_CHECK(error_code);

    error_code = nrf_ble_gatt_init(&gatt_instance, nullptr);
    APP_ERROR_CHECK(error_code);

    ble_advertising_init_t advertising_init_data;
    memset(&advertising_init_data, 0, sizeof(advertising_init_data));

    advertising_init_data.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advertising_init_data.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advertising_init_data.advdata.include_ble_device_addr = true;
    advertising_init_data.evt_handler = &advertising_event_handler;
    advertising_init_data.error_handler = &error_handler;
    advertising_init_data.config.ble_adv_fast_enabled = true;
    advertising_init_data.config.ble_adv_fast_timeout = 60;
    advertising_init_data.config.ble_adv_fast_interval = MSEC_TO_UNITS(200u, UNIT_0_625_MS);
    advertising_init_data.config.ble_adv_slow_enabled = true;
    advertising_init_data.config.ble_adv_slow_timeout = 0;
    advertising_init_data.config.ble_adv_slow_interval = MSEC_TO_UNITS(500u, UNIT_0_625_MS);

    error_code = ble_advertising_init(&advertising_instance, &advertising_init_data);
    APP_ERROR_CHECK(error_code);

    ble_advertising_conn_cfg_tag_set(&advertising_instance, 1);

    error_code = ble_advertising_start(&advertising_instance, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(error_code);

    error_code = pm_init();
    APP_ERROR_CHECK(error_code);

    ble_gap_sec_params_t security_parameter;
    memset(&security_parameter, 0, sizeof(security_parameter));
    security_parameter.bond = 1;
    security_parameter.mitm = 0;
    security_parameter.lesc = 0;
    security_parameter.keypress = 0;
    security_parameter.io_caps = BLE_GAP_IO_CAPS_NONE;
    security_parameter.oob = 0;
    security_parameter.min_key_size = 7;
    security_parameter.max_key_size = 16;
    security_parameter.kdist_own.enc = 1;
    security_parameter.kdist_own.id = 1;
    security_parameter.kdist_peer.enc = 1;
    security_parameter.kdist_peer.id = 1;

    error_code = pm_sec_params_set(&security_parameter);
    APP_ERROR_CHECK(error_code);

    error_code = pm_register(peer_manager_event_handler);
    APP_ERROR_CHECK(error_code);

    NRF_LOG_INFO("Starting the main loop");
    while (true) {
        if (!NRF_LOG_PROCESS()) {
            error_code = sd_app_evt_wait();
            APP_ERROR_CHECK(error_code);
        }
    }
}
