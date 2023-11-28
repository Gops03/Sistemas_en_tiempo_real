#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sensores.h"
#include "http_server.h"



int reset = 1;
int movimientoDetectado = 0;
int temperatura =0;


void comprobacion_task(void *pvParameters) {
    while (1) {
        int alarma = scanalarma();
        if (alarma == 1) {
            if (gpio_get_level(PIR_PIN) == 1) {
                // Movimiento detectado
                ESP_LOGI("PIR_SENSOR", "Movimiento detectado");
                gpio_set_level(BUZZER_PIN, 1); // Activa el buzzer
                movimientoDetectado = 1; // Indica que se ha detectado movimiento
            }
            else {
                ESP_LOGI("PIR_SENSOR", "Sin movimiento detectado");             
            }
        } else if (alarma == 0) {
            // Sin movimiento
            if (movimientoDetectado) {
                gpio_set_level(BUZZER_PIN, 0); // Apagar el buzzer si ya se ha detectado movimiento
                movimientoDetectado = 0; // Reiniciar el estado de detección de movimiento
            }
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);  // Pequeña espera para ceder CPU
    }
}

int temperaturas (){
    return temperatura;
}

