/*
 * servo_motor.h
 */
#ifndef INC_SERVO_MOTOR_H_
#define INC_SERVO_MOTOR_H_

#include "main.h" // Includes stm32l4xx.h

// --- TIMING CONSTANTS (Based on 4MHz Timer Clock) ---
// T_tick = 0.25 us/count. PWM Period = 20 ms (50 Hz)
#define PWM_PERIOD_MS        20000UL
#define PWM_FREQUENCY_COUNTS (PWM_PERIOD_MS * 4) // 80,000 counts for 20 ms @ 4MHz

// --- SG90 POSITION CONSTANTS (CCR values for TIM3->CCR4 on PB1) ---
// Note: Actual min/max is usually 1.0ms to 2.0ms
// 			divided by 2 because of prescaler

#define CLOSED (1000UL * 4) / 2
#define OPEN (500UL * 4) / 2

// --- Function Prototypes ---
void motor_init(void);
void TIM3_init(void);
void servo_set_angle(uint32_t ccr_value);
void motor_test_cycle(void); // <-- New function prototype

#endif /* INC_SERVO_MOTOR_H_ */
