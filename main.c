#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LED_PIN  PICO_DEFAULT_LED_PIN

// GP0-GP6 are used for button inputs
#define BUTTON_INPUT_MASK 0b1111111

#define PWM_PORT 28
#define PWM_SLICE 6
#define PWM_CHANNEL PWM_CHAN_A

#define MODT 562

void emit (unsigned int data, bool repeat){
	int i;
	// Allocate GPIO to the PWM
	gpio_set_function(PWM_PORT, GPIO_FUNC_PWM);
	// Set clock divier for frequency
	pwm_set_clkdiv(PWM_SLICE,125000.0/38000.0);
	// 1000 cycles PWM
	pwm_set_wrap(PWM_SLICE, 1000);
	// Set duty to zero
	pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 0);
	// Enable
	pwm_set_enabled(PWM_SLICE, true);
	
	// Begin
	pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 333);
	sleep_us(MODT*16);
	pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 0);
	sleep_us(MODT*8);
	
	// Send data
	for(i=0;i<32;i++){
		pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 333);
		sleep_us(MODT);
		pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 0);
		if (data & 0x80000000) sleep_us(MODT*3);
		else sleep_us(MODT);
		data<<=1;
	}
	
	// End
	pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 333);
	sleep_us(MODT);
	pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 0);
	
	// Repeat if needed
	while (repeat) {
		sleep_ms(100);
		pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 333);
		sleep_us(MODT*16);
		pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 0);
		sleep_us(MODT*4);
		pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 333);
		sleep_us(MODT);
		pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 0);
	}
	
	// Done
	pwm_set_enabled(PWM_SLICE, false);
	while(true) sleep_ms(1000);
}

int main() {
	int i;
	// Initialize buttons (input with pull down)
	gpio_init(BUTTON_INPUT_MASK);
	for(i=0;i<29;i++){
		if (BUTTON_INPUT_MASK & (1<<i)) gpio_pull_down(i);
	}
	// Wait for 30 msecs
	sleep_ms(30);
	// Check button
	i=gpio_get_all() & BUTTON_INPUT_MASK;
	if      (i & 0b0000001) emit(0x02fd58a7, true); // Volume up
	else if (i & 0b0000010) emit(0x02fd7887, true); // Volume down
	else if (i & 0b0000100) emit(0xa25dc837, true); // REW
	else if (i & 0b0001000) emit(0xa25da857, false); // Play/stop
	else if (i & 0b0010000) emit(0xa25d9867, true); // FF
	else if (i & 0b0100000) emit(0x02fd08f7, false); // Mute
	else if (i & 0b1000000) emit(0x02fd48b7, false); // Power

	while(true) sleep_ms(1000);
}
