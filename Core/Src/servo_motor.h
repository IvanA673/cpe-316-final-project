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

// --- SG90 POSITION CONSTANTS (CCR values for TIM3->CCR4 on PB2) ---
// Note: Actual min/max is usually 1.0ms to 2.0ms (4000 to 8000 counts)

// 1.0 ms pulse for 0 degrees (Left limit)
#define POS_0_DEGREES        (1000UL * 4) // 4,000 counts
// 1.5 ms pulse for 90 degrees (Center)
#define POS_90_DEGREES       (1500UL * 4) // 6,000 counts
// 2.0 ms pulse for 180 degrees (Right limit)
#define POS_180_DEGREES      (2000UL * 4) // 8,000 counts

// --- Function Prototypes ---
void motor_init(void);
void TIM3_init(void);
void servo_set_angle(uint32_t ccr_value);
void motor_test_cycle(void); // <-- New function prototype

#endif /* INC_SERVO_MOTOR_H_ */
