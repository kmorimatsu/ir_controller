#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LED_PIN  PICO_DEFAULT_LED_PIN

// GP0-GP6 are used for button inputs
#define BUTTON_INPUT_MASK 0b1111111

#define PWM_PORT 28
#define PWM_SLICE 6
#define PWM_CHANNEL PWM_CHAN_A

#define MODT 562

void wait562(int usecs){
	static unsigned int prev=0;
	unsigned int target;
	if (0==usecs) {
		// Init
		prev=time_us_32();
		return;
	}
	if (2<usecs) sleep_us(usecs-2);
	target=prev+usecs;
	while(time_us_32()<target);
	prev=target;
}

void wait108(void){
	static unsigned int prev=0;
	unsigned int target;
	if (!prev) {
		// Init
		prev=time_us_32();
		return;
	}
	target=prev+108000;
	while(time_us_32()<target) sleep_us(100);
	prev=target;
}

#define start_pwm() do {\
	pwm_set_counter(PWM_SLICE,0);\
	gpio_set_function(PWM_PORT, GPIO_FUNC_PWM);\
	pwm_set_enabled(PWM_SLICE, true);\
} while (false)

#define stop_pwm() do {\
	gpio_put(PWM_PORT,0);\
	gpio_set_function(PWM_PORT, GPIO_FUNC_SIO);\
	pwm_set_enabled(PWM_SLICE, false);\
} while (false)

void emit (unsigned int data, bool repeat){
	int i;
	// Fake signal in the beginning	
	stop_pwm();
	wait562(0);
	start_pwm();
	wait562(MODT*2);
	stop_pwm();
	wait562(MODT*2);
	start_pwm();
	wait562(MODT*2);
	stop_pwm();
	
	// Begin
	wait108();
	wait562(MODT*2);
	start_pwm();
	wait562(MODT*16);
	stop_pwm();
	wait562(MODT*8);
	
	// Send data
	for(i=0;i<32;i++){
		start_pwm();
		wait562(MODT);
		stop_pwm();
		if (data & 0x80000000) wait562(MODT*3);
		else wait562(MODT);
		data<<=1;
	}
	
	// End
	start_pwm();
	wait562(MODT);
	stop_pwm();
	
	// Repeat if needed
	while (repeat) {
		wait108();
		wait562(0);
		start_pwm();
		wait562(MODT*16);
		stop_pwm();
		wait562(MODT*4);
		start_pwm();
		wait562(MODT);
		stop_pwm();
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
	// Initialize PWM
	gpio_init(PWM_PORT);
	gpio_set_dir(PWM_PORT,GPIO_OUT);
	gpio_put(PWM_PORT,0);
	// Allocate GPIO to the SIO (not PWM yet)
	gpio_set_function(PWM_PORT, GPIO_FUNC_SIO);
	// Set clock divier for frequency
	pwm_set_clkdiv(PWM_SLICE,125000.0/38000.0);
	// 1000 cycles PWM
	pwm_set_wrap(PWM_SLICE, 1000);
	// Set duty to 33.3%
	pwm_set_chan_level(PWM_SLICE, PWM_CHANNEL, 333);
	// Enable PWM
	pwm_set_enabled(PWM_SLICE, true);;
	
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
