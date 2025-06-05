#include "v_bat.h"
#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>

#define ADC_BATTERY ADC_CHANNEL_7

static adc_cali_handle_t adc1_cali_chan1_handle = NULL;
static adc_oneshot_unit_handle_t adc1_handle;
static int adc_raw, adc_voltage;

void v_bat_init(void) {
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_BATTERY, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_chan1_handle));
}

float v_bat_read(void) {
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_BATTERY, &adc_raw));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan1_handle, adc_raw, &adc_voltage));
    adc_voltage *= 2;
    return (float) adc_voltage / 1000;
}
