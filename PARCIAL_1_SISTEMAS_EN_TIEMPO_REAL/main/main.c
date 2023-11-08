#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include <string.h>
#include "driver/adc.h"
#include <math.h> 

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE

#define LEDC_OUTPUT_IO1         (5)  // Define el GPIO de salida para LED1
#define LEDC_OUTPUT_IO2         (4)  // Define el GPIO de salida para LED2
#define LEDC_OUTPUT_IO3         (2)  // Define el GPIO de salida para LED3
#define BUTTON_PIN              (19) // Cambia esto al pin GPIO donde está conectado tu botón

#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_14_BIT
#define LEDC_FREQUENCY          (3000)
#define MAX_PRESSES              3

// Define la configuración UART
#define UART_NUM UART_NUM_0 
#define UART_TX_PIN  (GPIO_NUM_1) 
#define UART_RX_PIN (GPIO_NUM_3) 
#define BUF_SIZE (1024)
 
//Definicion de variables de control para el sensor NTC
#define SERIES_RESISTOR 47.0 // Valor de la resistencia de referencia en ohmios (47Ω)
#define beta 21 
#define referencia 3.3

// Prototipo de la función calcular_temperatura
float calcular_temperatura(uint32_t adc_value);

//INICIALIZACION DE LA VARIABLE DE CONTROL DEL RANGO DE LOS LEDS
int temperatura = 0;  // Variable de control inicializada en 0

// Frecuencia para mostrar la temperatura en el monitor de serie
int X = 1000;  // Cambia esta frecuencia según tus necesidades

// Duty cycle actual de los LEDs (inicializado al 100% para todos los LEDs)
int duty_cycle[3] = {100, 100, 100};


//FUNCION DE INICIALIZACION PARA EL ADC
void adc_init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12); // Configura la resolución a 12 bits
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // Configura el canal y la atenuación
}

//FUNCION DE INICIALIZACION PARA EL UART 
void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,           // Velocidad de baudios del UART
        .data_bits = UART_DATA_8_BITS, // 8 bits de datos
        .parity    = UART_PARITY_DISABLE, // Sin paridad
        .stop_bits = UART_STOP_BITS_1, // 1 bit de parada
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE // Control de flujo desactivado
    };

    // Configura el UART con los parámetros definidos
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));

    // Configura los pines GPIO para el UART
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Instala el controlador UART con una cola de recepción
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
}

//FUNCIONES DE NINICIALIZACION DE LOS LEDS...
void leds_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO1,
        .duty           = (1 << LEDC_DUTY_RES) - 1,  // Establece el deber al 100%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ledc_channel.channel = LEDC_CHANNEL_1;
    ledc_channel.gpio_num = LEDC_OUTPUT_IO2;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ledc_channel.channel = LEDC_CHANNEL_2;
    ledc_channel.gpio_num = LEDC_OUTPUT_IO3;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

//FUNCIONES DE INTERRUPCION PARA EL FUNCIONAMIENTO DEL BOTON 
void button_isr_handler(void* arg)
{
    X=X+1000;
    if (X>5000)
    {
        X=1000;
    } 
}

void configure_button_interrupt(void)
{
    gpio_config_t io_conf;
    // Configura el pin del botón como entrada
    io_conf.intr_type = GPIO_INTR_POSEDGE; // Interrupción en flanco de subida (cuando se presiona el botón)
    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    // Instala el servicio de interrupción GPIO
    gpio_install_isr_service(0);

    // Configura la interrupción del botón y maneja la interrupción con "button_isr_handler"
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);
}

// Tarea para recibir comandos y ajustar el duty cycle de los LEDs
void uart_command_task(void* arg)
{
    uint8_t* data = (uint8_t*)malloc(BUF_SIZE);
    while (1)
    {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            // Procesa los comandos recibidos aquí
            char command[32];
            strncpy(command, (char*)data, len);
            command[len] = '\0';

            int led_id = -1;
            int new_duty = -1;

            // Verifica si el comando tiene el formato correcto "D<LED_ID>:<DUTY>"
            if (sscanf(command, "D%d:%d", &led_id, &new_duty) == 2)
            {
                if (led_id >= 1 && led_id <= 3 && new_duty >= 0 && new_duty <= 100)
                {
                    // Actualiza el duty cycle del LED especificado
                    led_id--; // Ajusta el índice del LED (0-2 en lugar de 1-3)
                    duty_cycle[led_id] = new_duty;
                    printf("LED %d: Duty cycle actualizado a %d%%\n", led_id + 1, duty_cycle[led_id]);

                    // Actualiza el duty cycle de los LEDs
                    for (int i = 0; i < 3; i++)
                    {
                        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL + i, (duty_cycle[i] * ((1 << LEDC_DUTY_RES) - 1)) / 100);
                        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL + i);
                    }
                }
                else
                {
                    printf("Comando inválido: LED_ID debe estar entre 1 y 3, y DUTY debe estar entre 0 y 100\n");
                }
            }
            else
            {
                printf("Comando inválido: Formato incorrecto. Use D<LED_ID>:<DUTY>\n");
            }
        }
    }
    free(data);
    vTaskDelete(NULL);
}

//FUNCION PARA MOSTRAR LA VARIABLE DE TEMPERATURA 
void temperatura_display_task(void* arg)
{
    while (1)
    {
        printf("Temperatura: %d grados celcius\n", temperatura);  // Muestra la temperatura en el monitor de serie
        vTaskDelay(pdMS_TO_TICKS(X));
    }
}

//FUNCION PARA LA TEMPERATURA 
void temperatura_task(void* arg)
{
    while (1)
    {
       uint32_t adc_val=0;
       adc_val = adc1_get_raw(ADC1_CHANNEL_0);
       float adc_value = (float)adc_val;
       float voltajesensor = (referencia*adc_value)/4095;
       float resistencia = SERIES_RESISTOR / ((referencia/voltajesensor)-1);
       temperatura=(1/(log(resistencia/SERIES_RESISTOR)/beta))-273.15;//Formula para hallar la temperatura cen grados centigrados 
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera antes de la próxima lectura
    }
}

//CICLO PRINCIPAL
void app_main(void)
{
   leds_init(); 
   configure_button_interrupt(); 
   uart_init();
   adc_init(); // Inicializa el ADC
   
   // Crear una tarea para mostrar la temperatura en el monitor de serie
   xTaskCreate(temperatura_display_task, "temperatura_display_task", 2048, NULL, 5, NULL);

    // Crea una tarea para leer datos del UART
    xTaskCreate(uart_command_task, "uart_read_task", 2048, NULL, 5, NULL);

    // Crea una tarea para medir la temperatura
    xTaskCreate(temperatura_task, "temperatura_task", 2048, NULL, 5, NULL);

    //SE INICIA EL CICLO INFINITO
    while(1)
    {
        if (temperatura >= 0 && temperatura <= 30) {
            // Enciende LED1 al duty cycle configurado y apaga los otros
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, (duty_cycle[0] * ((1 << LEDC_DUTY_RES) - 1)) / 100);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
        } else if (temperatura > 30 && temperatura <= 50) {
            // Enciende LED2 al duty cycle configurado y apaga los otros
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, (duty_cycle[1] * ((1 << LEDC_DUTY_RES) - 1)) / 100);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
        } else {
            // Enciende LED3 al duty cycle configurado y apaga los otros
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, (duty_cycle[2] * ((1 << LEDC_DUTY_RES) - 1)) / 100);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // Espera 1 segundo antes de la próxima actualización
        
    }
}
