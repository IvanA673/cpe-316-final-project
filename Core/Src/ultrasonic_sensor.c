#include "main.h"
#include "ultrasonic_sensor.h"

// Define Global Variables
volatile uint8_t sample = FIRST_SAMPLE;
volatile uint32_t time_one = 0;
volatile uint32_t time_two = 0;
volatile uint32_t echo_pulse_duration_cnt = 0;
volatile uint8_t measurement_done_flag = 0;

void sensor_init(void) {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);

	/*----- Configure PB0 as sensor trigger (Output, Push-Pull) -----*/
	// setup MODER as output (01)
	GPIOB->MODER &= ~(GPIO_MODER_MODE0);
	GPIOB->MODER |= (GPIO_MODER_MODE0_0);
	// set push-pull output type (OTYPER bit 0)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0);
	// no pull-up/pull-down
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0);
	// set to high speed
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED0);

	/*----- PB3 setup (Echo, Alternate Function TIM2_CH2) -----*/
	// set to alternate function (10)
	GPIOB->MODER &= ~(GPIO_MODER_MODER3);
	GPIOB->MODER |= (GPIO_MODER_MODE3_1);
	// Map PB3 to AF1 (TIM2_CH2)
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL3;
	GPIOB->AFR[0] |= (1 << GPIO_AFRL_AFSEL3_Pos);
	// pull-down to ensure defined state before echo
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD3_1);
}

uint32_t measure_distance_cm(void) {
	uint32_t duration_counts = echo_pulse_duration_cnt;

	// Distance (cm) = (Duration_cnt * 42875) / 10000000
	uint64_t distance = duration_counts;
	distance = distance * 42875;
	distance = distance / 10000000;

	return (uint32_t)distance;
}

void TIM2_init(void) {
	RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);

	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);

	// TIM2 in up mode
	TIM2->CR1 &= ~(TIM_CR1_DIR);

	TIM2->ARR = MEASUREMENT_TIME;

	TIM2->CCR1 = TRIGGER_TIME;

	// Configure channel 2 (PB3/ECHO) as Input Capture
	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;

	// Set initial polarity to rising edge
	TIM2->CCER &= ~TIM_CCER_CC2P;

	TIM2->CCER |= TIM_CCER_CC2E;

	TIM2->DIER |= (TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE);

	TIM2->SR &= ~(TIM_SR_UIF | TIM_SR_CC1IF | TIM_SR_CC2IF);

	// Start Timer
	TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_CC1IF) {
		// end of 10us pulse
		GPIOB->ODR &= ~(GPIO_ODR_OD0); // Turn off TRIG pin (PB0 LOW)
		TIM2->SR &= ~(TIM_SR_CC1IF);
	}
	else if (TIM2->SR & TIM_SR_CC2IF) {
		// Input Capture event: triggered by transition on ECHO pin (PB3)

		uint32_t current_time = TIM2->CNT;

		if (sample == FIRST_SAMPLE) {
			// Rising edge detection
			time_one = current_time;
			sample = SECOND_SAMPLE;
			TIM2->CCER |= TIM_CCER_CC2P; // Switch polarity to look for Falling Edge
		} else if (sample == SECOND_SAMPLE) {
			// Falling edge detection
			time_two = current_time;

			if (time_two >= time_one) {
				echo_pulse_duration_cnt = time_two - time_one;
			}

			measurement_done_flag = 1;

			sample = FIRST_SAMPLE;
			TIM2->CCER &= ~TIM_CCER_CC2P; // Switch polarity back to Rising Edge
		}
		TIM2->SR &= ~(TIM_SR_CC2IF);
	}
	else if (TIM2->SR & TIM_SR_UIF) {

		// Check if previous ECHO pulse was still active (no measurement)
		if (sample == SECOND_SAMPLE) {
			// SCENARIO: Measurement failed to complete within 60ms cycle
			echo_pulse_duration_cnt = 0;

			sample = FIRST_SAMPLE;
			TIM2->CCER &= ~TIM_CCER_CC2P; // Switch polarity to Rising Edge
		}
		// start of 10us pulse every 60ms
		GPIOB->ODR |= (GPIO_ODR_OD0); // Turn on TRIG pin (PB0 High)
		TIM2->SR &= ~(TIM_SR_UIF);
	}
}
