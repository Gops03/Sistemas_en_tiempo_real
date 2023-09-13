#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/adc.h"

#define LED1_GPIO 2
#define LED2_GPIO 4
#define LED3_GPIO 5
#define BUTTOM_GPIO 26
#define POT_GPIO 34
#define ADC_CHANNEL ADC1_CHANNEL_6 // Cambiar al canal adecuado según la conexión del potenciómetro

uint32_t valorpotenciometro = 0;


//Definicion de los parametros de los leds
//Para el led 1
#define LED1_TIMER              LEDC_TIMER_0 //
#define LED1_MODE               LEDC_LOW_SPEED_MODE //Define el tipo de modo del led, en este caso baja velocidad
#define LED1_OUTPUT_IO          (2) // Define the output GPIO define el gpio
#define LED1_CHANNEL            LEDC_CHANNEL_0 //canal
#define LED1_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits

//Para el led 2
#define LED2_TIMER              LEDC_TIMER_0 //
#define LED2_MODE               LEDC_LOW_SPEED_MODE //Define el tipo de modo del led, en este caso baja velocidad
#define LED2_OUTPUT_IO          (39) // Define the output GPIO define el gpio
#define LED2_CHANNEL            LEDC_CHANNEL_1 //canal
#define LED2_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits

//Para el led 3
#define LED3_TIMER              LEDC_TIMER_0 //
#define LED3_MODE               LEDC_LOW_SPEED_MODE //Define el tipo de modo del led, en este caso baja velocidad
#define LED3_OUTPUT_IO          (36) // Define the output GPIO define el gpio
#define LED3_CHANNEL            LEDC_CHANNEL_2 //canal
#define LED3_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits


static void led_c_init (void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LED1_MODE,
        .timer_num        = LED1_TIMER,
        .duty_resolution  = LED1_DUTY_RES,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));//Confirma que todas las configuraciones iniciales fueron integradas correctamente 

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t led1_channel = {
        .speed_mode     = LED1_MODE, //Velocidad de trabajo
        .channel        = LED1_CHANNEL, //Canal de trabajo elegido, para esta tarjeta tienee 8 canales posibles 
        .timer_sel      = LED1_TIMER,//Seleccion del timer, debe ser el mismo timer que se  configuro anteriormente 
        .intr_type      = LEDC_INTR_DISABLE, //Esta interrupcuion avisa cuando la tarea se haya completado correctamente, esto tiene varias utilidades tecnicas
        .gpio_num       = LED1_OUTPUT_IO,
        .duty           = 0, // Set duty to 0% Selecciona el inicio de la tarea con el 0% de la energia del led 
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&led1_channel));

        // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t led2_channel = {
        .speed_mode     = LED2_MODE, //Velocidad de trabajo
        .channel        = LED2_CHANNEL, //Canal de trabajo elegido, para esta tarjeta tienee 8 canales posibles 
        .timer_sel      = LED2_TIMER,//Seleccion del timer, debe ser el mismo timer que se  configuro anteriormente 
        .intr_type      = LEDC_INTR_DISABLE, //Esta interrupcuion avisa cuando la tarea se haya completado correctamente, esto tiene varias utilidades tecnicas
        .gpio_num       = LED2_OUTPUT_IO,
        .duty           = 0, // Set duty to 0% Selecciona el inicio de la tarea con el 0% de la energia del led 
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&led2_channel));

        // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t led3_channel = {
        .speed_mode     = LED3_MODE, //Velocidad de trabajo
        .channel        = LED3_CHANNEL, //Canal de trabajo elegido, para esta tarjeta tienee 8 canales posibles 
        .timer_sel      = LED3_TIMER,//Seleccion del timer, debe ser el mismo timer que se  configuro anteriormente 
        .intr_type      = LEDC_INTR_DISABLE, //Esta interrupcuion avisa cuando la tarea se haya completado correctamente, esto tiene varias utilidades tecnicas
        .gpio_num       = LED3_OUTPUT_IO,
        .duty           = 0, // Set duty to 0% Selecciona el inicio de la tarea con el 0% de la energia del led 
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&led3_channel));
}

SemaphoreHandle_t xMutex; // Objeto de semaforo
QueueHandle_t buffer;     // Objeto de la cola

void adcyduty(void *pvParameters)
{
    while (1)
    {
        // Leer el valor del potenciómetro y actualizar el brillo del LED
        uint32_t valor_potenciometro = adc1_get_raw(ADC_CHANNEL);
        int duty = (int)(valor_potenciometro / 40.95); // Escalar el rango de 0-4095 a 0-100
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeño retardo para controlar la frecuencia de actualización
    }
}

void lec_boton(void)
{
    gpio_set_direction(BUTTOM_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTOM_GPIO, GPIO_PULLUP_ONLY);

    int button_state = 1; // Inicialmente, suponemos que el botón está en alto (no presionado)

    while (1)
    {
        // Verificar el estado del botón
        int new_button_state = gpio_get_level(BUTTOM_GPIO);
        int led_state=1;

        // Comprobar si el botón ha cambiado de estado
        if (new_button_state != button_state)
        {
            button_state = new_button_state;

            if (button_state == 0) // Botón presionado (nivel bajo)
            {
                // Incrementar el estado del LED y reiniciar si es mayor que 3
                led_state++;
                if (led_state > 3) {
                    led_state = 1;
                }

                // Apagar todos los LEDs
                gpio_set_level(LED1_GPIO, 0);
                gpio_set_level(LED2_GPIO, 0);
                gpio_set_level(LED3_GPIO, 0);

                // Encender el LED correspondiente según el valor del contador
                switch (led_state) {
                    case 1:
                        gpio_set_level(LED1_GPIO, 1);
                        break;
                    case 2:
                        gpio_set_level(LED2_GPIO, 1);
                        break;
                    case 3:
                        gpio_set_level(LED3_GPIO, 1);
                        break;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // Pequeño retardo para evitar rebotes del botón
    }
}


void app_main(void)
{
    // Set the LEDC peripheral configuration
    led_c_init();
    lec_boton();
    xTaskCreate(adcyduty, "adcyduty_task", 2048, NULL, 5, NULL);


}

