/*
 * rgb_led.c
 *
 *  Created on: Oct 11, 2021
 *      Author: kjagu
 */

#include <stdbool.h>

#include "driver/ledc.h"
#include "rgb_led.h"

// RGB LED Configuration Array
ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM];

// handle for rgb_led_pwm_init
bool g_pwm_init_handle = false;

/**
 * Initializes the RGB LED settings per channel, including
 * the GPIO for each color, mode and timer configuration.
 */
static void rgb_led_pwm_init(void)
{
	int rgb_ch;

	// Red1
	ledc_ch[0].channel		= LEDC_CHANNEL_0;
	ledc_ch[0].gpio			= RGB_LED_RED1_GPIO;
	ledc_ch[0].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[0].timer_index	= LEDC_TIMER_0;

	// Green1
	ledc_ch[1].channel		= LEDC_CHANNEL_1;
	ledc_ch[1].gpio			= RGB_LED_GREEN1_GPIO;
	ledc_ch[1].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[1].timer_index	= LEDC_TIMER_0;

	// Blue1
	ledc_ch[2].channel		= LEDC_CHANNEL_2;
	ledc_ch[2].gpio			= RGB_LED_BLUE1_GPIO;
	ledc_ch[2].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[2].timer_index	= LEDC_TIMER_0;

	// Red2
	ledc_ch[3].channel		= LEDC_CHANNEL_3;
	ledc_ch[3].gpio			= RGB_LED_RED2_GPIO;
	ledc_ch[3].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[3].timer_index	= LEDC_TIMER_0;

	// Green2
	ledc_ch[4].channel		= LEDC_CHANNEL_4;
	ledc_ch[4].gpio			= RGB_LED_GREEN2_GPIO;
	ledc_ch[4].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[4].timer_index	= LEDC_TIMER_0;

	// Blue2
	ledc_ch[5].channel		= LEDC_CHANNEL_5;
	ledc_ch[5].gpio			= RGB_LED_BLUE2_GPIO;
	ledc_ch[5].mode			= LEDC_HIGH_SPEED_MODE;
	ledc_ch[5].timer_index	= LEDC_TIMER_0;
	
	// Configure timer zero
	ledc_timer_config_t ledc_timer =
	{
		.duty_resolution	= LEDC_TIMER_8_BIT,
		.freq_hz			= 100,
		.speed_mode			= LEDC_HIGH_SPEED_MODE,
		.timer_num			= LEDC_TIMER_0
	};
	ledc_timer_config(&ledc_timer);

	// Configure channels
	for (rgb_ch = 0; rgb_ch < RGB_LED_CHANNEL_NUM; rgb_ch++)
	{
		ledc_channel_config_t ledc_channel =
		{
			.channel	= ledc_ch[rgb_ch].channel,
			.duty		= 0,
			.hpoint		= 0,
			.gpio_num	= ledc_ch[rgb_ch].gpio,
			.intr_type	= LEDC_INTR_DISABLE,
			.speed_mode = ledc_ch[rgb_ch].mode,
			.timer_sel	= ledc_ch[rgb_ch].timer_index,
		};
		ledc_channel_config(&ledc_channel);
	}

	g_pwm_init_handle = true;
}

/**
 * Sets the RGB color.
 */
void rgb_led_set_color(uint8_t red1, uint8_t green1, uint8_t blue1, uint8_t red2, uint8_t green2, uint8_t blue2)
{
	// Value should be 0 - 255 for 8 bit number
	ledc_set_duty(ledc_ch[0].mode, ledc_ch[0].channel, red1);
	ledc_update_duty(ledc_ch[0].mode, ledc_ch[0].channel);

	ledc_set_duty(ledc_ch[1].mode, ledc_ch[1].channel, green1);
	ledc_update_duty(ledc_ch[1].mode, ledc_ch[1].channel);

	ledc_set_duty(ledc_ch[2].mode, ledc_ch[2].channel, blue1);
	ledc_update_duty(ledc_ch[2].mode, ledc_ch[2].channel);

	ledc_set_duty(ledc_ch[3].mode, ledc_ch[3].channel, red2);
	ledc_update_duty(ledc_ch[3].mode, ledc_ch[3].channel);

	ledc_set_duty(ledc_ch[4].mode, ledc_ch[4].channel, green2);
	ledc_update_duty(ledc_ch[4].mode, ledc_ch[4].channel);

	ledc_set_duty(ledc_ch[5].mode, ledc_ch[5].channel, blue2);
	ledc_update_duty(ledc_ch[5].mode, ledc_ch[5].channel);
}

void rgb_led_wifi_app_started(void)
{
	if (g_pwm_init_handle == false)
	{
		rgb_led_pwm_init();
	}

	rgb_led_set_color(255, 102, 255, 255, 255, 255);
}

void rgb_led_http_server_started(void)
{
	if (g_pwm_init_handle == false)
	{
		rgb_led_pwm_init();
	}

	rgb_led_set_color(255, 102, 255, 255, 255, 255);
}


void rgb_led_wifi_connected(void)
{
	if (g_pwm_init_handle == false)
	{
		rgb_led_pwm_init();
	}

	rgb_led_set_color(255, 102, 255, 255, 255, 255);
}

 
void LUZHABITACION1(void){
	// Restablece el estado del pin
    gpio_set_level(RGB_LED_RED1_GPIO, 0);
}   














































