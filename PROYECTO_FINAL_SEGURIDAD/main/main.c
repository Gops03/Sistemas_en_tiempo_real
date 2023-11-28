/**
 * Application entry point.
 */

#include "nvs_flash.h"
#include "driver/gpio.h"
#include "wifi_app.h"
#include "rgb_led.h"
#include "http_server.h"
#include "control_uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensores.h"

void app_main(void)
{
	// Configurar el pin del zumbador como salida
    gpio_pad_select_gpio(BUZZER_PIN);
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);

    // Configurar el pin del sensor PIR como entrada con pull-down
    gpio_pad_select_gpio(PIR_PIN);
    gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT);

	uart_init();
	xTaskCreate(comprobacion_task, "comprobacion_task", 4096, NULL, 5, NULL);
	
    // Crea una tarea para leer datos del UART
    xTaskCreate(uart_command_task, "uart_read_task", 2048, NULL, 5, NULL);
    // Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);


	// Start Wifi
	wifi_app_start();
}

