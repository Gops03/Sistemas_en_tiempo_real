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
void uart_command_task(void *arg)
{
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    const char *bienvenida = "Bienvenido al centro de mando. \n";
    const char *mensaje1 = "A continuacion digite su numero de superusuario para acceder a los controles: \n";
    const char *accede = "ACCESO CONFIRMADO. \n";
    const char *noaccede = "ACCESO DENEGADO. \n";
    const char *mensaje2 = "Por favor introduce el nuevo usuario. \n";
    const char *mensaje3 = "El nuevo usuario es:. \n";
    const char *mensaje4 = "Por favor introduce la nueva contraseña. \n";
    const char *mensaje5 = "la nueva contraseña es:. \n";
    char usuario[32];  // Variable para almacenar el nombre de usuario
    char contrasena[32];  // Variable para almacenar la contraseña

    while (1)
    {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            char command[32];
            strncpy(command, (char *)data, len);
            command[len] = '\0';

            // Envía datos a través de UART directamente en la tarea
            uart_write_bytes(UART_NUM, bienvenida, strlen(bienvenida));
            uart_write_bytes(UART_NUM, mensaje1, strlen(mensaje1));

            // Verifica si el comando es para encender el LED
            if (strcmp(command, "1234") == 0)
            {
                uart_write_bytes(UART_NUM, accede, strlen(accede));
                uart_write_bytes(UART_NUM, mensaje2, strlen(mensaje2));

                // Espera a recibir el nombre de usuario
                len = uart_read_bytes(UART_NUM, data, BUF_SIZE, portMAX_DELAY);
                strncpy(usuario, (char *)data, len);
                usuario[len] = '\0';
                uart_write_bytes(UART_NUM, mensaje3, strlen(mensaje3));
                uart_write_bytes(UART_NUM, usuario, strlen(usuario));


                // Espera a recibir la contraseña
                uart_write_bytes(UART_NUM, mensaje4, strlen(mensaje4));
                len = uart_read_bytes(UART_NUM, data, BUF_SIZE, portMAX_DELAY);
                strncpy(contrasena, (char *)data, len);
                contrasena[len] = '\0';
                uart_write_bytes(UART_NUM, mensaje5, strlen(mensaje5));
                uart_write_bytes(UART_NUM, contrasena, strlen(contrasena));

                // Ahora usuario y contraseña contienen la información ingresada
                // Puedes hacer lo que necesites con esta información

            }
            else
            {
                uart_write_bytes(UART_NUM, noaccede, strlen(noaccede));
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