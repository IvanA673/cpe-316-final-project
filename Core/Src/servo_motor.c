#include "servo_motor.h"


void motor_init(void) {
	// Enable GPIOB clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	/*---- Configure PB1 as motor signal (TIM3_CH4) -----*/

	// 2. Setup PB1 as Alternate Function (10)
	GPIOB->MODER &= ~GPIO_MODER_MODE1;
	GPIOB->MODER |= GPIO_MODER_MODE1_1;

	// 3. Map PB1 to AF2 (TIM3_CH4)
	// AFR[0] controls pins 0-7. AFSEL1 field (bits 4-7) controls PB1.
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL1;
	GPIOB->AFR[0] |= (2 << GPIO_AFRL_AFSEL1_Pos); // Set AF2 (0010)

	// 4. Push-pull output type and High Speed
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT1);
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED1);
}

void TIM3_init(void) {
		// Enable TIM3 clock
		RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	    // Set Prescaler to 1 to divide timer clock by 2
		TIM3->PSC = 1;

	    // Set ARR for 50Hz (80,000 counts / 2)
		TIM3->ARR = 39999;

		// Set PWM Mode 1 (OC4M = 110) on Channel 4
		TIM3->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
		TIM3->CCMR2 |= (6 << TIM_CCMR2_OC4M_Pos);

	    // Enable Preload for CCR4 (allows seamless duty cycle changes)
		TIM3->CCMR2 |= TIM_CCMR2_OC4PE;

	    // Enable Capture/Compare Channel 4 Output (CC4E)
		TIM3->CCER |= TIM_CCER_CC4E;

	    // Set initial duty cycle to NEUTRAL (90 degrees)
		TIM3->CCR4 = CLOSED;

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
