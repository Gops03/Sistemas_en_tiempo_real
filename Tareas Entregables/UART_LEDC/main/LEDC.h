

#define LEDC_OUTPUT_IO1         (5)  // Define el GPIO de salida para LED1
#define LEDC_OUTPUT_IO2         (4)  // Define el GPIO de salida para LED2
#define LEDC_OUTPUT_IO3         (2)  // Define el GPIO de salida para LED3
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE

#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_14_BIT
#define LEDC_FREQUENCY          (3000)

void leds_init(void);