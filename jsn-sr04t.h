/**
 * @file jsn-sr04t.h
 *
 * JSN-SR04T ultrasonic module driver.
 *
 * @author Michal Horn
 */

#ifndef JSN_SR04T_JSN_SR04T_H_
#define JSN_SR04T_JSN_SR04T_H_

/**
 * States of the driver
 */
typedef enum {
	JSN_READY,              //!< The module is ready to measure.
	JSN_WAITING_FOR_TRIGGER,//!< Measuring state - waiting for trigger to start.
	JSN_TRIGGER_UP,         //!< Measuring state - creating the 10us trigger pulse.
	JSN_WAITING_FOR_ECHO,   //!< Measuring state - waiting for the echo rising edge.
	JSN_ECHO_UP,            //!< Measuring state - waiting for the echo falling edge.
	JSN_FINISHED,			//!< Measuring has finished successfully.
	JSN_FAILURE,            //!< Measuring failed due to a timeout in any of the measuring states.
} jsn_sr04t_state_t;

/**
 * Driver API functions return values.
 */
typedef enum {
	JSN_SUCCESS,//!< Function exited with success.
	JSN_ERROR   //!< Error occurred during the function execution.
} jsn_sr04t_ret_val_t;

/**
 * Module descriptor.
 */
typedef struct jsn_sr04t_desc_st {
	jsn_sr04t_state_t state;	//!< The state of the module.
	unsigned int counter_10us;	//!< The timer counter for the module, counts in 10s of micro seconds.
	void* trigger_port;			//!< The GPIO port of trigger pin.
	void* echo_port;			//!< The GPIO port of echo pin.
	void* trigger_pin;			//!< The GPIO trigger pin.
	void* echo_pin;				//!< The GPIO echo pin.
	unsigned int distance_cm;	//!< The calculated distance in cm. 0 in case of failure.
} jsn_sr04t_desc_t;

unsigned int JSN_GPIO_Get_value_wrapper(void* GPIO_Port, void* GPIO_Pin);

void JSN_GPIO_Set_value_wrapper(void* GPIO_Port, void* GPIO_Pin, unsigned int value);

/**
 * Initialize the driver for the ultrasonic module.
 *
 * Must be called for every ultrasonic module used in the application.
 *
 * @param desc The module descriptor pointer to be initialized.
 * @param t_port The GPIO port to be used for trigger pin.
 * @param t_pin The GPIO port to be used for echo pin.
 * @param e_port The GPIO trigger pin.
 * @param e_pin The GPIO echo pin.
 */
void JSN_Init(jsn_sr04t_desc_t* desc, void* t_port, void* t_pin, void* e_port, void* e_pin);

/**
 * Initiate measuring.
 *
 * @param desc Ultrasonic module descriptor.
 * @return JSN_SUCCESS on success or JSN_ERROR if previous measuring not finished.
 */
jsn_sr04t_ret_val_t JSN_Measure(jsn_sr04t_desc_t* desc);

/**
 * Get the ultrasonic module state.
 *
 * @param desc Ultrasonic module descriptor.
 * @return The state of the ultrasonic module.
 */
jsn_sr04t_state_t JSN_GetState(jsn_sr04t_desc_t* desc);

/**
 * Return true if the measuring has finished successfully or with an error.
 *
 * @param desc Ultrasonic module descriptor.
 * @return 1 if the measuring has finished, 0 if the measuring is still running.
 */
int JSN_Measuring_finished(jsn_sr04t_desc_t* desc);

/**
 * Calculate and return the distance in centimeters.
 * @param desc Ultrasonic module descriptor.
 * @return The calculated distance in cm or 0 if the module is in JSN_FAILURE state.
 */
unsigned int JSN_GetDistance_cm(jsn_sr04t_desc_t* desc);

/**
 * Callback for 10us timer.
 *
 * This callback has to be called from the ISR of a Timer with period of 10 microseconds.
 * It has to be called for every ultrasonic module used in the application.
 * @param desc Ultrasonic module descriptor.
 */
void JSN_10us_timer_callback(jsn_sr04t_desc_t* desc);

/**
 * Callback for External Interrupts.
 *
 * This callback has to be called from the ISR of EXTI, attached to the echo pin GPIO.
 * The interrupt has to be generated on both, rising and falling edges.
 * This callback has to be called for every ultrasonic modules used in the application.
 * @param desc Ultrasonic module descriptor.
 */
void JSN_GPIO_EXTI_callback(jsn_sr04t_desc_t* desc);

#endif /* JSN_SR04T_JSN_SR04T_H_ */
