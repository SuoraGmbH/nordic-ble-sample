#include <app_timer.h>
#include <bsp.h>
#include <nrf_delay.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>
#include <nrf_sdh.h>

int main() {
    ret_code_t error_code;

    // common init
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

    error_code = bsp_indication_set(BSP_INDICATE_ALERT_1);
    APP_ERROR_CHECK(error_code);

    // main loop
    NRF_LOG_INFO("Starting the main loop");
    while (true) {
        if (!NRF_LOG_PROCESS()) {
            error_code = sd_app_evt_wait();
            APP_ERROR_CHECK(error_code);
        }
    }
}
