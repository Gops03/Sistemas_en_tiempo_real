#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sensores.h"
#include "http_server.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "nvs_flash.h"
#include "rgb_led.h"



int reset = 1;
int movimientoDetectado = 0;
float temperatura =0.0;


void comprobacion_task(void *pvParameters) {
    while (1) {
        int alarma = scanalarma();
        if (alarma == 1) {
            if (gpio_get_level(PIR_PIN) == 1) {
                // Movimiento detectado
                ESP_LOGI("PIR_SENSOR", "Movimiento detectado");
                sirenaa(255);
                movimientoDetectado = 1; // Indica que se ha detectado movimiento
            }
            else {
                ESP_LOGI("PIR_SENSOR", "Sin movimiento detectado");             
            }
        } else if (alarma == 0) {
            // Sin movimiento
            if (movimientoDetectado) {
                movimientoDetectado = 0; // Reiniciar el estado de detección de movimiento
            }
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);  // Pequeña espera para ceder CPU
    }
}



static esp_adc_cal_characteristics_t *adc_chars;

void check_efuse(void) {
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

void init_adc(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LM35_CHANNEL, ADC_ATTEN_DB_11);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
}

void comprobacion2_task(void *pvParameters)
{
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    check_efuse();
    init_adc();

    while (1) {
        uint32_t adc_reading = 0;

        // Muestreo múltiple para promedio
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading += adc1_get_raw(LM35_CHANNEL);
        }
        adc_reading /= NO_OF_SAMPLES;

        // Convertir adc_reading a voltaje en mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        temperatura = (float)voltage / 10; // LM35 produce 10 mV por grado Celsius
        temperaturas();

        vTaskDelay(pdMS_TO_TICKS(1000));  // Retardo de 1 segundo
    }
}

float temperaturas (){
    return temperatura;
}