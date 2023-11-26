#include "control_uart.h"
#include "driver/uart.h"
#include <string.h>
#include "driver/ledc.h"
#include "rgb_led.h"
#include <stdio.h>
#include "http_server.h"


extern int led_id=0;
extern int new_duty=0;

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

            led_id = -1;
            new_duty = -1;

            // Verifica si el comando tiene el formato correcto "D<LED_ID>:<DUTY>"
            if (sscanf(command, "D%d:%d", &led_id, &new_duty) == 2)
            {
                if (led_id >= 1 && led_id <= 6 && new_duty >= 0 && new_duty <= 255)
                {
                    // Actualiza el duty cycle del LED especificado
                    led_id--; // Ajusta el índice del LED (0-5 en lugar de 1-6)
                    //duty_cycle[led_id] = new_duty;
                    //printf("LED %d: Duty cycle actualizado a %d%%\n", led_id + 1, duty_cycle[led_id]);
                    
                    switch (led_id)
                    {
                    case 0:
                        rgb_led_set_color(new_duty, 0, 0, 0, 0, 0);
                        printf("LED %d: Duty cycle actualizado\n", led_id + 1);  
                        scan();                                  
                        break;
                    case 1:
                        rgb_led_set_color(0, new_duty, 0, 0, 0, 0);
                        printf("LED %d: Duty cycle actualizado\n", led_id + 1);
                        scan();
                        break;
                    case 2:
                        rgb_led_set_color(0, 0, new_duty, 0, 0, 0);
                        printf("LED %d: Duty cycle actualizado\n", led_id + 1);
                        scan();
                        break;                    
                    case 3:
                        rgb_led_set_color(0, 0, 0, new_duty, 0, 0);
                        printf("LED %d: Duty cycle actualizado\n", led_id + 1);
                        scan();
                        break;
                    case 4:
                        rgb_led_set_color(0, 0, 0, 0, new_duty, 0);
                        printf("LED %d: Duty cycle actualizado\n", led_id + 1);
                        scan();
                        break;                    
                    case 5:
                        rgb_led_set_color(0, 0, 0, 0, 0, new_duty);
                        printf("LED %d: Duty cycle actualizado\n", led_id + 1);
                        scan();
                        break;
                    default:
                        break;
                    }

                }
                else
                {
                    printf("Comando inválido: LED_ID debe estar entre 1 y 6, y DUTY debe estar entre 0 y 255\n");
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

int scan (){
    return led_id;
}

int scan2 (){
    return new_duty;
}