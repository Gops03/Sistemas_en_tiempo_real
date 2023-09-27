#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h" // Agrega la librería para UART
#include <string.h>

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE

#define LEDC_OUTPUT_IO1         (5)  // Define the output GPIO for LED1
#define LEDC_OUTPUT_IO2         (4)  // Define the output GPIO for LED2
#define LEDC_OUTPUT_IO3         (2)  // Define the output GPIO for LED3
#define MAX_PRESSES              3

#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_14_BIT // Set duty resolution to 14 bits
#define LEDC_FREQUENCY          (3000)            // Frequency in Hertz. Set frequency at 5 kHz
#define POTENTIOMETER_PIN       (34)              // Pin to which the potentiometer is connected
#define BUTTON_PIN              (26)               // Cambia esto al pin GPIO donde está conectado tu botón

// Define la configuración UART
#define UART_NUM UART_NUM_0 // Puedes cambiar el número de UART si es necesario
#define UART_BAUD_RATE 115200
#define TXD_PIN (GPIO_NUM_1) // Reemplaza GPIO_NUM_4 con el número de pin que desees
#define RXD_PIN (GPIO_NUM_3) // Reemplaza GPIO_NUM_5 con el número de pin que desees
#define LED_RANGE_START 'R' // Nuevo prefijo para identificar comandos de rango
#define LED_RANGE_LENGTH 6 // Longitud total del comando de rango (incluido 'R' y otros caracteres)
#define UART_BUFFER_SIZE 128 // Cambia el tamaño según tus necesidades
#define portTICK_RATE_MS portTICK_PERIOD_MS
 
//definicion del sensor ntc
#define NTC_PIN ADC1_CHANNEL_6  // Cambiar al canal ADC correcto en tu ESP32
#define RESISTOR_VALUE 5.1       // Valor de la resistencia en ohmios
#define NOMINAL_RESISTANCE 47.0  // Valor nominal de resistencia a 25°C del NTC

static const char *TAG = "Potentiometer_LED";

static void example_ledC1_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO1, // Use LED1 as the initial LED
        .duty           = 0,               // Set initial duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void example_ledC2_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO2, // Use LED1 as the initial LED
        .duty           = 0,               // Set initial duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void example_ledC3_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO3, // Use LED1 as the initial LED
        .duty           = 0,               // Set initial duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

    int press_count = 0; 
void button_task(void *pvParameter)
{

    while (1)
    {
        if (gpio_get_level(BUTTON_PIN) == 0) // Verifica si el botón está presionado (nivel bajo)
        {
            // Espera a que el botón se suelte antes de contar otra pulsación
            while (gpio_get_level(BUTTON_PIN) == 0)
            {
                vTaskDelay(10 / portTICK_PERIOD_MS); // Pequeña espera para evitar rebotes
            }

            press_count++; // Incrementa el contador de pulsaciones
            printf("Pulsación %d\n", press_count);

            if (press_count >= MAX_PRESSES)
            {
                press_count = 0; // Reinicia el contador de pulsaciones
                printf("Contador reiniciado\n");
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // Pequeña espera para evitar rebotes
    }
}

// Función para configurar un LED con un rango dado
void configureLedRange(ledc_channel_t led_channel, int superior, int inferior)
{
    // Implementa la lógica para configurar el rango del LED según el nuevo formato
    // Asegúrate de validar los rangos adecuadamente.
}

void processUartCommand(const char *command)
{
    if (strlen(command) != LED_RANGE_LENGTH)
    {
        ESP_LOGW(TAG, "Comando UART de longitud incorrecta: %s", command);
        return;
    }

    if (command[0] != LED_RANGE_START)
    {
        ESP_LOGW(TAG, "Comando UART no comienza con '%c': %s", LED_RANGE_START, command);
        return;
    }

    // Extraer el color del LED y los valores superior e inferior del comando
    char color = command[1];
    int superior = atoi(&command[3]);
    int inferior = atoi(&command[8]);

    ESP_LOGI(TAG, "Configurando LED de color '%c' con rango superior=%d e inferior=%d", color, superior, inferior);

    // Llama a la función que configura el LED con los valores extraídos
    // Implementa esta función según tu configuración de LED específica
    // Por ejemplo: configureLedRange(LEDC_CHANNEL, superior, inferior);
}

void uartTask(void *pvParameter)
{
    uint8_t *data = (uint8_t *)malloc(UART_BUFFER_SIZE);

    while (1)
    {
        int len = uart_read_bytes(UART_NUM, data, UART_BUFFER_SIZE, 20 / portTICK_RATE_MS);
        if (len > 0)
        {
            data[len] = 0; // Null-terminate los datos recibidos para tratarlos como una cadena de caracteres
            ESP_LOGI(TAG, "Recibido por UART: %s", data);

            // Procesar el comando UART
            processUartCommand((const char *)data);
        }
    }
    free(data);
}

void app_main(void)
{
    // Configura el UART de entrada
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, 1024, 0, 0, NULL, 0));


    // Set the LEDC peripheral configuration
    example_ledC1_init();
    example_ledC2_init();
    example_ledC3_init();

    // Configure the ADC to read the potentiometer value
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    esp_rom_gpio_pad_select_gpio(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    xTaskCreate(&button_task, "button_task", 2048, NULL, 5, NULL);

    while (1)
    {
        // Read the potentiometer value
        unsigned int potentiometer_value = adc1_get_raw(ADC1_CHANNEL_6);

        // Map the potentiometer value to the LED duty cycle (0-4095)
        unsigned int duty = (potentiometer_value * 4095) / 4095;
        
        // Leer datos del UART de entrada
        uint8_t uart_data;
        if (uart_read_bytes(UART_NUM, &uart_data, 1, portMAX_DELAY) == 1)
        {
            // Si se recibe un '1' a través del UART, realiza alguna acción
            if (uart_data == '1')
            {
                // Realiza la acción deseada, por ejemplo, cambiar el LED
                ESP_LOGI(TAG, "Recibido '1' a través del UART, realizando acción.");
            }
        }


        switch (press_count) {
    case 1:
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)); // LED3

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1)); // LED3

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2)); // LED3

        
        break;

    case 2:
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)); // LED3

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, duty));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, duty)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, duty)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1)); // LED3

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2)); // LED3

        break;

    default:
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL)); // LED3

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1)); // LED3

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, duty));
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, duty)); // LED2
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, duty)); // LED3

        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2)); // LED2
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2)); // LED3
}
  

        ESP_LOGI(TAG, "Potentiometer Value: %u, Duty: %u", potentiometer_value, duty);

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for stability
    }
}