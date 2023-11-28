// Define la configuración UART
#define UART_NUM UART_NUM_0 
#define UART_TX_PIN  (GPIO_NUM_1) 
#define UART_RX_PIN (GPIO_NUM_3) 
#define BUF_SIZE (1024)

void uart_init(void);
void uart_command_task(void* arg);

int scan (void);
int scan2 (void);
char* user (void);
char* contra (void);