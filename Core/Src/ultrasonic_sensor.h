/*
 * ultrasonic_sensor.h
 */
#ifndef INC_ULTRASONIC_SENSOR_H_
#define INC_ULTRASONIC_SENSOR_H_

#include "main.h" // Assumes main.h provides stm32l4xx.h, stdint.h, etc.

// --- Sensor Timing Constants (For 4MHz clock) ---
#define MEASUREMENT_TIME (240000UL)   // 60ms cycle (ARR value)
#define TRIGGER_TIME     (40UL)       // 10us pulse width (CCR1 value)

// --- State Machine Definitions ---
#define FIRST_SAMPLE     0
#define SECOND_SAMPLE    1

// --- Global Variables (Defined in ultrasonic_sensor.c, declared extern here) ---
// These are written to by the ISR and read by the main loop.
extern volatile uint8_t sample;
extern volatile uint32_t time_one;
extern volatile uint32_t time_two;
extern volatile uint32_t echo_pulse_duration_counts;
extern volatile uint8_t measurement_done_flag;

// --- Function Prototypes ---
void sensor_init(void);
uint32_t measure_distance_cm(void);
void TIM2_init(void);

#endif /* INC_ULTRASONIC_SENSOR_H_ */
