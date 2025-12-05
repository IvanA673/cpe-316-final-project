#include "main.h"
#include "ultrasonic_sensor.h"
#include "servo_motor.h"

volatile uint32_t current_time_ms = 0;

int main() {
	HAL_Init();

	sensor_init();
	motor_init();
	TIM2_init();
	TIM3_init();

	__enable_irq();

	uint32_t distance_cm = 0;

	// Local variables for state management
	uint32_t gate_open_timer = 0;

	// 0: Gate Closed, 1: Gate Open/Detecting, 2: Timed Hold Active
	uint8_t gate_state = 0;

	while (1) {
		if (measurement_done_flag) {
			// Disabling interrupts while calculating prevents race conditions
			__disable_irq();
			distance_cm = measure_distance_cm();
			measurement_done_flag = 0;
			__enable_irq();
		}

		// Within desired range
		if (distance_cm > 0 && distance_cm <= 6) {
			servo_set_angle(OPEN);
			gate_state = 1;
		}
		// 2. TRANSITION: Object just left (State 1) -> Start the hold timer
		else if (gate_state == 1) {

			// Object is gone (distance > 6 cm) AND gate was just open (state 1)
			// Calculate the time 3 seconds (3000 ms) into the future
			gate_open_timer = current_time_ms + 3000;
			gate_state = 2; // Transition to the timed hold state
		}
		// 3. TIMED HOLD: Object left, but we are waiting for the timer to expire (State 2)
		else if (gate_state == 2) {

			// Check if 3 seconds (3000 ms) have elapsed
			if (current_time_ms >= gate_open_timer) {

				// Hold time expired: Close the gate and return to closed state.
				servo_set_angle(CLOSED);
				gate_state = 0;
			}

		}
	}
}
