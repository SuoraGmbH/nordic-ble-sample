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

std::uint32_t read_counter = 0;

std::array<std::uint8_t, 4> write_buffer = {};

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

void ble_event_handler(const ble_evt_t *event, void *) {
    ret_code_t error_code;

    switch (event->header.evt_id) {
    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("Client disconnected");
        break;

    case BLE_GAP_EVT_CONNECTED:
        NRF_LOG_INFO("Client connected");
        error_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
        APP_ERROR_CHECK(error_code);
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        NRF_LOG_ERROR("GATT client timeout");
        error_code = sd_ble_gap_disconnect(event->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(error_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        NRF_LOG_ERROR("GATT server timeout");
        error_code = sd_ble_gap_disconnect(event->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(error_code);
        break;

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: {
        if (event->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ) {
            ++read_counter;

            ble_gatts_rw_authorize_reply_params_t reply{};
            memset(&reply, 0, sizeof(reply));
            reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
            reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
            reply.params.read.update = 1;
            reply.params.read.offset = 0;
            reply.params.read.len = sizeof(read_counter);
            reply.params.read.p_data = reinterpret_cast<const std::uint8_t *>(&read_counter);

            error_code = sd_ble_gatts_rw_authorize_reply(event->evt.gatts_evt.conn_handle, &reply);
            APP_ERROR_CHECK(error_code);
        } else if (event->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {
            const ble_gatts_evt_write_t &write_event = event->evt.gatts_evt.params.authorize_request.request.write;

            ble_gatts_rw_authorize_reply_params_t reply{};
            memset(&reply, 0, sizeof(reply));
            reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
            reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
            reply.params.write.update = 1;

            std::array<int, 4> written_data = {};
            for (std::size_t i = 0; i < write_event.len && i < written_data.size(); ++i) {
                written_data[i] = write_event.data[write_event.offset + i];
            }

            NRF_LOG_INFO("%i bytes written: %02x%02x%02x%02x", write_event.len, static_cast<int>(written_data[0]),
                         static_cast<int>(written_data[1]), static_cast<int>(written_data[2]),
                         static_cast<int>(written_data[3]));

            error_code = sd_ble_gatts_rw_authorize_reply(event->evt.gatts_evt.conn_handle, &reply);
            APP_ERROR_CHECK(error_code);
        }
    } break;

    case BLE_EVT_USER_MEM_REQUEST:
        error_code = sd_ble_user_mem_reply(event->evt.common_evt.conn_handle, nullptr);
        APP_ERROR_CHECK(error_code);
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

    std::uint32_t application_ram_start_address;
    error_code = nrf_sdh_ble_default_cfg_set(1, &application_ram_start_address);
    APP_ERROR_CHECK(error_code);

    error_code = nrf_sdh_ble_enable(&application_ram_start_address);
    APP_ERROR_CHECK(error_code);

    NRF_SDH_BLE_OBSERVER(ble_observer, 2, &ble_event_handler, nullptr);

    error_code = nrf_ble_gatt_init(&gatt_instance, nullptr);
    APP_ERROR_CHECK(error_code);

    // Define the UUIDs
    // 95c2xxxx-0b95-40e0-a903-7b8e1be9a64b
    ble_uuid128_t base_uuid_data{
        {0x95, 0xc2, 0x00, 0x00, 0x0b, 0x95, 0x40, 0xe0, 0xa9, 0x03, 0x7b, 0x8e, 0x1b, 0xe9, 0xa6, 0x4b}};
    std::uint8_t base_uuid_type;
    error_code = sd_ble_uuid_vs_add(&base_uuid_data, &base_uuid_type);
    APP_ERROR_CHECK(error_code);

    ble_uuid_t service_uuid;
    service_uuid.type = base_uuid_type;
    service_uuid.uuid = 0;

    ble_uuid_t read_characteristic_uuid;
    read_characteristic_uuid.type = base_uuid_type;
    read_characteristic_uuid.uuid = 1;

    ble_uuid_t write_characteristic_uuid;
    read_characteristic_uuid.type = base_uuid_type;
    read_characteristic_uuid.uuid = 2;

    // Add the service
    std::uint16_t service_handle;
    error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &service_handle);
    APP_ERROR_CHECK(error_code);

    // Add the read characteristic
    ble_gatts_char_md_t read_characteristic_metadata;
    memset(&read_characteristic_metadata, 0, sizeof(read_characteristic_metadata));
    read_characteristic_metadata.char_props.read = 1;

    ble_gatts_attr_md_t read_write_attribute_metadata;
    memset(&read_write_attribute_metadata, 0, sizeof(read_write_attribute_metadata));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&read_write_attribute_metadata.read_perm);
    read_write_attribute_metadata.vloc = BLE_GATTS_VLOC_STACK;
    read_write_attribute_metadata.rd_auth = 1;

    ble_gatts_attr_t read_write_attribute;
    memset(&read_write_attribute, 0, sizeof(read_write_attribute));
    read_write_attribute.p_uuid = &read_characteristic_uuid;
    read_write_attribute.p_attr_md = &read_write_attribute_metadata;
    read_write_attribute.init_len = 0;
    read_write_attribute.max_len = sizeof(read_counter);
    read_write_attribute.init_offs = 0;
    read_write_attribute.p_value = nullptr;

    ble_gatts_char_handles_t read_characteristic_handle;

    error_code = sd_ble_gatts_characteristic_add(service_handle, &read_characteristic_metadata, &read_write_attribute,
                                                 &read_characteristic_handle);
    APP_ERROR_CHECK(error_code);

    // Add the write characteristic
    ble_gatts_char_md_t write_characteristic_metadata;
    memset(&write_characteristic_metadata, 0, sizeof(write_characteristic_metadata));
    write_characteristic_metadata.char_props.write = 1;

    ble_gatts_attr_md_t write_write_attribute_metadata;
    memset(&write_write_attribute_metadata, 0, sizeof(write_write_attribute_metadata));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&write_write_attribute_metadata.write_perm);
    write_write_attribute_metadata.vloc = BLE_GATTS_VLOC_USER;
    write_write_attribute_metadata.wr_auth = 1;

    ble_gatts_attr_t write_attribute;
    memset(&write_attribute, 0, sizeof(write_attribute));
    write_attribute.p_uuid = &write_characteristic_uuid;
    write_attribute.p_attr_md = &write_write_attribute_metadata;
    write_attribute.init_len = 0;
    write_attribute.max_len = write_buffer.size();
    write_attribute.init_offs = 0;
    write_attribute.p_value = write_buffer.data();

    ble_gatts_char_handles_t write_characteristic_handle;

    error_code = sd_ble_gatts_characteristic_add(service_handle, &write_characteristic_metadata, &write_attribute,
                                                 &write_characteristic_handle);
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

    ble_advertising_init_t advertising_init_data;
    memset(&advertising_init_data, 0, sizeof(advertising_init_data));

    advertising_init_data.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advertising_init_data.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advertising_init_data.advdata.include_ble_device_addr = true;
    advertising_init_data.evt_handler = &advertising_event_handler;
    advertising_init_data.error_handler = &abort_on_error;
    advertising_init_data.config.ble_adv_fast_enabled = true;
    advertising_init_data.config.ble_adv_fast_timeout = 60;
    advertising_init_data.config.ble_adv_fast_interval = MSEC_TO_UNITS(200u, UNIT_0_625_MS);
    advertising_init_data.config.ble_adv_slow_enabled = true;
    advertising_init_data.config.ble_adv_slow_timeout = 0;
    advertising_init_data.config.ble_adv_slow_interval = MSEC_TO_UNITS(500u, UNIT_0_625_MS);

    auto service_uuids = std::array<ble_uuid_t, 1>{service_uuid};
    advertising_init_data.srdata.uuids_complete.p_uuids = service_uuids.data();
    advertising_init_data.srdata.uuids_complete.uuid_cnt = service_uuids.size();

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
