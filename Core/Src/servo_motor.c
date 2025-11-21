#include "servo_motor.h"


void motor_init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	// Configure PB2 as motor signal (TIM3_CH4)
	/* Why? --> hardware handles setting PWM signal once
		CCR4 value is reached. PB2 AF motor mode mapped to
		channel 4 in SMT
	*/

	// Set PB2 as AF mode
	GPIOB->MODER &= ~GPIO_MODER_MODE2;
	GPIOB->MODER |= GPIO_MODER_MODE2_1;

	// Map PB2 to AF2 (TIM3_CH4)
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL2;
	GPIOB->AFR[0] |= (2 << GPIO_AFRL_AFSEL2_Pos);

	// Push-pull output type
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT2);
}

void TIM3_init(void) {
		// Enable TIM3 clock
		RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	    // Set Prescaler to 0
		TIM3->PSC = 0;

	    // Set ARR for 50Hz (80,000 counts)
		TIM3->ARR = PWM_FREQUENCY_COUNTS;

	    // Set PWM Mode 1 (OC4M = 110) on Channel 4
		TIM3->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
		TIM3->CCMR2 |= (6 << TIM_CCMR2_OC4M_Pos);

	    // Enable Preload for CCR4 (allows seamless duty cycle changes)
		TIM3->CCMR2 |= TIM_CCMR2_OC4PE;

	    // Enable Capture/Compare Channel 4 Output (CC4E)
		TIM3->CCER |= TIM_CCER_CC4E;

	    // Set initial duty cycle to NEUTRAL (90 degrees)
		TIM3->CCR4 = POS_90_DEGREES;

	    // Enable ARR Preload
		TIM3->CR1 |= TIM_CR1_ARPE;

	    // Generate an Update Event (UG) to load registers
		TIM3->EGR |= TIM_EGR_UG;

	    // Start the timer (CEN)
		TIM3->CR1 |= TIM_CR1_CEN;
}

void servo_set_angle(uint32_t ccr_value) {
	// CCR4 Register controls the PWM pulse width, which sets servo angle
	TIM3->CCR4 = ccr_value;
}

void motor_test_cycle(void) {
    const uint32_t DELAY_MS = 1500; // 1.5 seconds for visual confirmation
    uint32_t next_event_time = 0;
    uint8_t test_stage = 0;

    // Set initial position to 0 degrees before starting the loop
    servo_set_angle(POS_0_DEGREES);
    next_event_time = current_time_ms + DELAY_MS;

    // Test loop runs four times to confirm full range and returns to center
    while (test_stage < 4) {

        // Wait until the delay time has passed
        if (current_time_ms >= next_event_time) {

            // Set the next position based on the stage
            switch (test_stage) {
                case 0: // Move to 90 degrees
                    servo_set_angle(POS_90_DEGREES);
                    break;
                case 1: // Move to 180 degrees
                    servo_set_angle(POS_180_DEGREES);
                    break;
                case 2: // Move back to 0 degrees
                    servo_set_angle(POS_0_DEGREES);
                    break;
                case 3: // Return to 90 degrees (resting center)
                    servo_set_angle(POS_90_DEGREES);
                    break;
            }

            // Reset the timer and advance the stage
            next_event_time = current_time_ms + DELAY_MS;
            test_stage++;
        }

        // IMPORTANT: The main loop must continue to run in the background.
        // We will call this test function in main()'s while(1) loop.
    }
}
