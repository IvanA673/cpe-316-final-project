#include "main.h"
#include "ultrasonic_sensor.h"

int main() {
	HAL_Init();
	sensor_init();
	TIM2_init();

	uint32_t distance_cm = 0;

	while (1) {
		if (measurement_done_flag) {
			distance_cm = measure_distance_cm();
			measurement_done_flag = 0;
			if (distance_cm <= 6) {
				//logic
			}
		}
	}
}
