/**
 * @file jsn-sr04t.c
 *
 * JSN-SR04T ultrasonic module driver.
 *
 * @author Michal Horn
 */

#include "jsn-sr04t.h"

#define JSN_TRIGGER_TIME_10US	1
#define JSN_ECHO_TIMEOUT_10US	3200

void JSN_Init(jsn_sr04t_desc_t* desc, GPIO_TypeDef* t_port, uint16_t t_pin, GPIO_TypeDef* e_port, uint16_t e_pin) {
	desc->counter_10us = 0;
	desc->trigger_port = t_port;
	desc->trigger_pin = t_pin;
	desc->echo_port = e_port;
	desc->echo_pin = e_pin;
	desc->distance_cm = 0;
	desc->state = JSN_READY;
}

jsn_sr04t_ret_val_t JSN_Measure(jsn_sr04t_desc_t* desc) {
	if (desc->state != JSN_READY && desc->state != JSN_FAILURE) {
		return JSN_ERROR;
	}

	desc->state = JSN_WAITING_FOR_TRIGGER;

	return JSN_SUCCESS;
}

jsn_sr04t_state_t JSN_GetState(jsn_sr04t_desc_t* desc) {
	return desc->state;
}

uint32_t JSN_GetDistance_cm(jsn_sr04t_desc_t* desc) {
	if (desc->state == JSN_READY) {
		desc->distance_cm = (desc->counter_10us*10) >> 6;
	}
	else {
		desc->distance_cm = 0;
	}
	return desc->distance_cm;
}

void JSN_10us_timer_callback(jsn_sr04t_desc_t* desc) {
	switch (desc->state) {
	case JSN_WAITING_FOR_TRIGGER:
		desc->counter_10us = 0;
		desc->state = JSN_TRIGGER_UP;
		HAL_GPIO_WritePin(desc->trigger_port, desc->trigger_pin, GPIO_PIN_SET);
		break;
	case JSN_TRIGGER_UP:
		desc->counter_10us++;
		if (desc->counter_10us >= JSN_TRIGGER_TIME_10US) {
			desc->state = JSN_WAITING_FOR_ECHO;
			HAL_GPIO_WritePin(desc->trigger_port, desc->trigger_pin, GPIO_PIN_RESET);
			desc->counter_10us = 0;
		}
		break;
	case JSN_WAITING_FOR_ECHO:
	case JSN_ECHO_UP:
		desc->counter_10us++;
		if (desc->counter_10us >= JSN_ECHO_TIMEOUT_10US) {
			desc->state = JSN_FAILURE;
		}
		break;
	default:
		break;
	}
}

void JSN_GPIO_EXTI_callback(jsn_sr04t_desc_t* desc) {
	GPIO_PinState pin_value;
	switch (desc->state) {
	case JSN_WAITING_FOR_ECHO:
		pin_value = HAL_GPIO_ReadPin(desc->echo_port, desc->echo_pin);
		if (pin_value == GPIO_PIN_SET) {
			desc->distance_cm = 0;
			desc->counter_10us = 0;
			desc->state = JSN_ECHO_UP;
		}
		else {
			desc->state = JSN_FAILURE;
		}
		break;
	case JSN_ECHO_UP:
		pin_value = HAL_GPIO_ReadPin(desc->echo_port, desc->echo_pin);
		if (pin_value == GPIO_PIN_RESET) {
			desc->state = JSN_READY;
		}
		else {
			desc->state = JSN_FAILURE;
		}
		break;
	default:
		break;
	}
}
