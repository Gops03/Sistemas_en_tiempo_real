#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include <string.h>
#include <math.h> // Necesario para la función log

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE

#define LEDC_OUTPUT_IO1         (5)  // Define el GPIO de salida para LED1
#define LEDC_OUTPUT_IO2         (4)  // Define el GPIO de salida para LED2
#define LEDC_OUTPUT_IO3         (2)  // Define el GPIO de salida para LED3

#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_14_BIT // Resolución del ciclo de trabajo a 14 bits
#define LEDC_FREQUENCY          (3000)            // Frecuencia en Hertz. Configura la frecuencia a 5 kHz

#define SENSOR_NTC_PIN          ADC1_CHANNEL_6 // Cambiar al canal ADC correcto en tu ESP32

// Define la configuración UART
#define UART_NUM UART_NUM_0 // Puedes cambiar el número de UART si es necesario
#define UART_BAUD_RATE 115200
#define TXD_PIN (GPIO_NUM_1) // Reemplaza GPIO_NUM_1 con el número de pin que desees
#define RXD_PIN (GPIO_NUM_3) // Reemplaza GPIO_NUM_3 con el número de pin que desees

#define LED_RANGE_START 'R' // Nuevo prefijo para identificar comandos de rango
#define LED_RANGE_LENGTH 6 // Longitud total del comando de rango (incluido 'R' y otros caracteres)
#define UART_BUFFER_SIZE 128 // Cambia el tamaño según tus necesidades
#define portTICK_RATE_MS portTICK_PERIOD_MS

static const char *TAG = "Potentiometer_LED";

static void example_ledC1_init(void)
{
    // Preparar y luego aplicar la configuración del temporizador PWM LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Configurar la frecuencia de salida a 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Preparar y luego aplicar la configuración del canal PWM LEDC
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO1, // Usar LED1 como LED inicial
        .duty           = 0,               // Configurar el ciclo de trabajo inicial al 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void example_ledC2_init(void)
{
    // Preparar y luego aplicar la configuración del temporizador PWM LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Configurar la frecuencia de salida a 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Preparar y luego aplicar la configuración del canal PWM LEDC
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO2, // Usar LED2 como LED inicial
        .duty           = 0,               // Configurar el ciclo de trabajo inicial al 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void example_ledC3_init(void)
{
    // Preparar y luego aplicar la configuración del temporizador PWM LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Configurar la frecuencia de salida a 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Preparar y luego aplicar la configuración del canal PWM LEDC
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO3, // Usar LED3 como LED inicial
        .duty           = 0,               // Configurar el ciclo de trabajo inicial al 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

// Asumiendo que tienes un valor de resistencia fija y el valor de resistencia del NTC a 25°C
float fixedResistance = 100.0; // Valor de resistencia fija en ohmios
float ntcResistance25 = 47.0;  // Valor de resistencia del NTC a 25°C en ohmios

// Función para calcular la temperatura en grados Celsius
float calculateTemperature(unsigned int adcValue)
{
    float resistance = (fixedResistance * adcValue) / (4095.0 - adcValue);
    float steinhart;
    
    // Utiliza la ecuación de Steinhart-Hart para calcular la temperatura
    steinhart = resistance / ntcResistance25;     // (R/Ro)
    steinhart = log(steinhart);                    // ln(R/Ro)
    steinhart /= 3950.0;                           // 1/B * ln(R/Ro)
    steinhart += 1.0 / (25.0 + 273.15);            // + (1/To)
    steinhart = 1.0 / steinhart;                   // Invierte
    steinhart -= 273.15;                           // Convierte a Celsius
    
    return steinhart;
}

void app_main(void)
{
    // Configura el UART de entrada
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, 1024, 0, 0, NULL, 0));

    // Configurar la inicialización de los LEDs PWM
    example_ledC1_init();
    example_ledC2_init();
    example_ledC3_init();

    // Configura el ADC para leer el valor del sensor NTC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(SENSOR_NTC_PIN, ADC_ATTEN_DB_11);

    while (1)
    {
        // Leer el valor del sensor NTC
        unsigned int ntc_value = adc1_get_raw(SENSOR_NTC_PIN);

        // Calcular la temperatura en grados Celsius
        float temperature = calculateTemperature(ntc_value);

        // Controlar los LEDs en función de la temperatura
        if (temperature <= 15.0)
        {
            // Encender LED1 y apagar los demás
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);  // Apagar LEDC_CHANNEL
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 4095);  // Encender LEDC_CHANNEL_1
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0);  // Apagar LEDC_CHANNEL_2
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
        }
        else if (temperature <= 30.0)
        {
            // Apagar LED1, Encender LED2, Apagar LED3
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);  // Apagar LEDC_CHANNEL
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0);  // Apagar LEDC_CHANNEL_1
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 4095);  // Encender LEDC_CHANNEL_2
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
        }
        else
        {
            // Apagar LED1, Apagar LED2, Encender LED3
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);  // Apagar LEDC_CHANNEL
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0);  // Apagar LEDC_CHANNEL_1
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0);  // Apagar LEDC_CHANNEL_2
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
        }

        // ... (código de manejo de UART y presiones, si es necesario)

        vTaskDelay(pdMS_TO_TICKS(1000)); // Retardo para evitar cambios rápidos
    }
}
