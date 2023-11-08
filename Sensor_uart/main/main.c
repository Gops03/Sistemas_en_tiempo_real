#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define TXD_PIN (UART_PIN_NO_CHANGE) // Usar el pin por defecto para TX
#define RXD_PIN (UART_PIN_NO_CHANGE) // Usar el pin por defecto para RX
#define UART_NUM UART_NUM_1          // UART1

// Pines GPIO para los LEDs
#define LED_BLANCO GPIO_NUM_26
#define LED_VERDE GPIO_NUM_27
#define LED_ROJO GPIO_NUM_25

// Variables de control para los comandos
char command1[64];
char command2[64];
char command3[64];

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 115200,             // Velocidad de baudios
        .data_bits = UART_DATA_8_BITS,   // 8 bits de datos
        .parity    = UART_PARITY_DISABLE,// Sin paridad
        .stop_bits = UART_STOP_BITS_1,   // 1 bit de parada
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    // Configurar UART con la configuración especificada
    uart_param_config(UART_NUM, &uart_config);
    // Configurar los pines para TX y RX
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // Instalar el controlador UART
    uart_driver_install(UART_NUM, 1024, 0, 0, NULL, 0);
}

void init_leds() {
    
    gpio_set_direction(LED_BLANCO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_VERDE, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_ROJO, GPIO_MODE_OUTPUT);
    
    gpio_set_level(LED_BLANCO, 0); // Apagar el LED blanco al inicio
    gpio_set_level(LED_VERDE, 0);  // Apagar el LED verde al inicio
    gpio_set_level(LED_ROJO, 0);   // Apagar el LED rojo al inicio
}

void uart_task() {
    while (1) {
        // Leer datos del UART
        uint8_t data;
        if (uart_read_bytes(UART_NUM, &data, 1, portMAX_DELAY)) {
            // Procesar los datos recibidos
            printf("Dato recibido: %c\n", data);
            
            // Si recibe un '1', enviar la palabra "funcionó" por el UART
            if (data == '1') {
                const char *message = "funcionó\n"; // Agregar '\n' para indicar el final del mensaje
                uart_write_bytes(UART_NUM, message, strlen(message));
            }
            
            // Resto del código para procesar otros comandos
            static int command_index = 0;
            if (data == '\n' || data == '\r') {
                command_index = 0; // Reiniciar el índice del comando
            } else if (command_index < sizeof(command1) - 1) {
                command1[command_index++] = data;
                command1[command_index] = '\0'; // Asegurarse de que el comando sea una cadena C válida
            }
            
            // Procesar otros comandos para encender o apagar LEDs
            if (strcmp(command1, "blanco_encender") == 0) {
                gpio_set_level(LED_BLANCO, 1);
            } else if (strcmp(command1, "blanco_apagar") == 0) {
                gpio_set_level(LED_BLANCO, 0);
            } else if (strcmp(command1, "verde_encender") == 0) {
                gpio_set_level(LED_VERDE, 1);
            } else if (strcmp(command1, "verde_apagar") == 0) {
                gpio_set_level(LED_VERDE, 0);
            } else if (strcmp(command1, "rojo_encender") == 0) {
                gpio_set_level(LED_ROJO, 1);
            } else if (strcmp(command1, "rojo_apagar") == 0) {
                gpio_set_level(LED_ROJO, 0);
            }
        }
    }
}

void app_main() {
    uart_init(); // Inicializar UART
    init_leds(); // Inicializar los LEDs
    xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL); // Crear tarea de UART
    
    while (1) {
        uart_task();
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar un segundo antes de imprimir de nuevo
    }
}
