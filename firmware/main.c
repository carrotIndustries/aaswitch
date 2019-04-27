#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 1000000
#include <util/delay.h>
#include <stdint.h>
#include <avr/eeprom.h>

//extern const __flash uint8_t gammatab[256];

#define ABS(a) (((a) < 0) ? -(a) : (a))

#define KEEP_PRELOAD 1000

const uint8_t dimtab[] = {255, 127, 64, 4};

void out_init() {
	DDRB |= (1<<PB0) | (1<<PB1);
}

void out_set(uint8_t x) {
	if(x)
		PORTB |= (1<<PB1);
	else
		PORTB &= ~(1<<PB1);
}

void led_sig_set(uint8_t x) {
	if(x)
		PORTB |= (1<<PB0);
	else
		PORTB &= ~(1<<PB0);
}

typedef enum {
	M_AUTO,
	M_ON,
	M_OFF
} mode_t;

void inc_clamp(uint16_t *x) {
	if(*x < 0xfffe)
		(*x)++;
}

int main(void) {
	out_init();
	out_set(1);
	ADMUX = 3 | (1<<REFS1);
	ADCSRA |= (1<<ADEN) | (1<<ADPS0);
	uint16_t prescaler = 1;
	uint16_t amin[2] = {0xffff};
	uint16_t amax[2] = {0xffff};
	uint8_t ch = 0;
	uint8_t mode = M_ON;
	uint16_t counter_on = 0;
	uint16_t counter_off = 0;
	
	while(1) {
		if(ch == 0) {
			ADMUX = 3 | (1<<REFS1);
		}
		else {
			ADMUX = 2 | (1<<REFS1);
		}
		
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		int16_t result = ADCW;
		if(amax[ch] == 0xffff || amin[ch] == 0xffff) {
			amin[ch] = result;
			amax[ch] = result;
		}
		else {
			if(result < amin[ch])
				amin[ch] = result;
			if(result > amax[ch])
				amax[ch] = result;
		}
			
		prescaler--;
		if(prescaler == 0) {
			uint8_t sig = ((amax[0] - amin[0]) > 20) || ((amax[1] - amin[1]) > 20);
			if(sig) {
				inc_clamp(&counter_on);
				counter_off = 0;
			}
			else {
				inc_clamp(&counter_off);
				counter_on = 0;
			}
			led_sig_set(sig);
			switch(mode) {
				case M_ON:
					out_set(1);
				break;
				
				case M_OFF:
					out_set(0);
				break;
				
				case M_AUTO:
					if(counter_on > 5) {
						out_set(1);
					}
					else if(counter_off > 3750) {
						out_set(0);
					}
				
					
				break;
				
			}
			prescaler = 1024;
			amin[0] = 0xffff;
			amin[1] = 0xffff;
			amax[0] = 0xffff;
			amax[1] = 0xffff;
			
			ADMUX = 1;
			
			//dummy conversion
			ADCSRA |= (1<<ADSC);
			while(ADCSRA & (1<<ADSC));
			
			ADCSRA |= (1<<ADSC);
			while(ADCSRA & (1<<ADSC));
			
			uint16_t r = ADCW;
			if(r > 682) {
				mode = M_ON;
			}
			else if(r > 341) {
				mode = M_OFF;
			}
			else {
				mode = M_AUTO;
			}
			
			//dummy conversion
			ADMUX = (1<<REFS1);
			ADCSRA |= (1<<ADSC);
			while(ADCSRA & (1<<ADSC));
			
		}
		ch = !ch;
		
	}
	return 0;
}
