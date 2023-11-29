/*
 * rgb_led.h
 *
 *  Created on: Oct 11, 2021
 *      Author: kjagu
 */

#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_

// RGB LED GPIOs
#define RGB_LED_RED1_GPIO		16
#define RGB_LED_GREEN1_GPIO		17
#define RGB_LED_BLUE1_GPIO		5
#define RGB_LED_RED2_GPIO		4
#define RGB_LED_GREEN2_GPIO		2
#define RGB_LED_BLUE2_GPIO		18

// RGB LED color mix channels
#define RGB_LED_CHANNEL_NUM		6

// RGB LED configuration
typedef struct
{
	int channel;
	int gpio;
	int mode;
	int timer_index;
} ledc_info_t;
// ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM]; Move this declaration to the top of rgb_led.c to avoid linker errors

/**
 * Color to indicate WiFi application has started.
 */
void rgb_led_wifi_app_started(void);

/**
 * Color to indicate HTTP server has started.
 */
void rgb_led_http_server_started(void);

/**
 * Color to indicate that the ESP32 is connected to an access point.
 */
void rgb_led_wifi_connected(void);
void rgb_led_set_color(uint8_t red1, uint8_t green1, uint8_t blue1, uint8_t red2, uint8_t green2, uint8_t blue2);
void luzhabitacionprincipal(uint8_t v);
void luzentrada(uint8_t v);
void neveraa(uint8_t v);
void persianashabitacionprincipal(uint8_t v);
void persianassala(uint8_t v);
void sirenaa(uint8_t v);

#endif /* MAIN_RGB_LED_H_ */
