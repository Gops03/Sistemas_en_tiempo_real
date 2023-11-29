
#define PIR_PIN GPIO_NUM_19
#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          // Multisampling
#define LM35_CHANNEL    ADC1_CHANNEL_0 // GPIO 36 (ESP32) ADC1_CHANNEL_0 es el canal para GPIO36



//Definir las funciones que se van a usar 
void comprobacion_task(void *pvParameters);
int gpio_pad_select_gpio();
float temperaturas (void);
void check_efuse(void);
void init_adc(void);
void comprobacion2_task(void *pvParameters);