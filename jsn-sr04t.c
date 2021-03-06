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

void JSN_Init(jsn_sr04t_desc_t* desc, void* t_port, void* t_pin, void* e_port, void* e_pin) {
	desc->counter_10us = 0;
	desc->trigger_port = t_port;
	desc->trigger_pin = t_pin;
	desc->echo_port = e_port;
	desc->echo_pin = e_pin;
	desc->distance_cm = 0;
	desc->state = JSN_READY;
}

jsn_sr04t_ret_val_t JSN_Measure(jsn_sr04t_desc_t* desc) {
	if (desc->state != JSN_FINISHED && desc->state != JSN_READY && desc->state != JSN_FAILURE) {
		return JSN_ERROR;
	}

	desc->state = JSN_WAITING_FOR_TRIGGER;

	return JSN_SUCCESS;
}

jsn_sr04t_state_t JSN_GetState(jsn_sr04t_desc_t* desc) {
	return desc->state;
}

int JSN_Measuring_finished(jsn_sr04t_desc_t* desc) {
	if (desc->state == JSN_FINISHED || desc->state == JSN_FAILURE) {
		return 1;
	}
	else {
		return 0;
	}
}


unsigned int JSN_GetDistance_cm(jsn_sr04t_desc_t* desc) {
	if (desc->state == JSN_FINISHED) {
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
		JSN_GPIO_Set_value_wrapper(desc->trigger_port, desc->trigger_pin, 1);
		break;
	case JSN_TRIGGER_UP:
		desc->counter_10us++;
		if (desc->counter_10us >= JSN_TRIGGER_TIME_10US) {
			desc->state = JSN_WAITING_FOR_ECHO;
			JSN_GPIO_Set_value_wrapper(desc->trigger_port, desc->trigger_pin, 0);
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
	unsigned int pin_value;
	switch (desc->state) {
	case JSN_WAITING_FOR_ECHO:
		pin_value = JSN_GPIO_Get_value_wrapper(desc->echo_port, desc->echo_pin);
		if (pin_value == 1) {
			desc->distance_cm = 0;
			desc->counter_10us = 0;
			desc->state = JSN_ECHO_UP;
		}
		else {
			desc->state = JSN_FAILURE;
		}
		break;
	case JSN_ECHO_UP:
		pin_value = JSN_GPIO_Get_value_wrapper(desc->echo_port, desc->echo_pin);
		if (pin_value == 0) {
			desc->state = JSN_FINISHED;
		}
		else {
			desc->state = JSN_FAILURE;
		}
		break;
	default:
		break;
	}
}
