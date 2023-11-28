#define BUZZER_PIN GPIO_NUM_13
#define PIR_PIN GPIO_NUM_22

//Definir las funciones que se van a usar 
void comprobacion_task(void *pvParameters);
int gpio_pad_select_gpio();
int temperaturas (void);
