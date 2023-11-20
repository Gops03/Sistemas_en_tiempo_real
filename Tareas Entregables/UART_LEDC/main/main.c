#include <stdio.h>
#include "CUART.h"
#include "LEDC.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void app_main(void)
{
    uart_init();
    leds_init(); 
    // Crea una tarea para leer datos del UART
    xTaskCreate(uart_command_task, "uart_read_task", 2048, NULL, 5, NULL);

}