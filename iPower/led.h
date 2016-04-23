
#ifndef LED_H
#define LED_H

#define LED_OFF			0
#define LED_RED			1
#define LED_GREEN		2

const int blink_delay_start = 3000; //ms
const int blink_delay_on = 800; 	//ms
const int blink_delay_off = 400; 	//ms

class Led
{
public:
	Led( int _pin_green, int _pin_red ) {
		pin_green = _pin_green;
		pin_red = _pin_red;

		// initialize led pins
    	pinMode(_pin_green, OUTPUT);
    	pinMode(_pin_red, OUTPUT);
        blink_count = 0;
        previous_state = state = 0;
	};

	void set_blink( int _state, int _blink_count ) {
		previous_state = state;
		state = _state;
		blink_count = _blink_count;
		time = 0;
	};

	void set( int _state ) {
		set_blink(_state, 0);
	};

	void update( void ) {
		// return if nothing update
		if( previous_state == state && blink_count == 0 ) 
			return;

		if( blink_count == 0 ) {
			// reset time
			time = 0;
		}

		// blinking if need
		if( blink_count > 0 ) {

			if( time == 0 ) {
				time = millis();
				// do off
				write(LED_OFF);
				return;
			} 

			unsigned long delay = millis() - time;

			if( previous_state == LED_OFF && delay < blink_delay_start ) {
				// wait 'off' delay
				return;
			} 

			if( previous_state == LED_OFF && delay >= blink_delay_start ) {
				// enable led
				write(state);
				time = millis();
				return;
			}

			if ( previous_state != LED_OFF && delay >= blink_delay_on ) {
				// do off
				write(LED_OFF);
				// shift time for short off
				time = millis() - (blink_delay_start - blink_delay_off);
				blink_count --;
				return;
			} 
			else {
				// wait 'on' delay
				return;
			}
		}
		
		// change led
		write(state);
	};

private:
	int pin_green;
	int pin_red;
	int state;
	int previous_state;
	int blink_count;
	unsigned long time;

	void write( int state ) {
		if( state == LED_RED ) {
			// enable red led
	    	digitalWrite(pin_red, HIGH);
	    	digitalWrite(pin_green, LOW);
		} else
			if( state == LED_GREEN ) {
				// enable green led
		    	digitalWrite(pin_green, HIGH);
		    	digitalWrite(pin_red, LOW);
			} else
				if( state == LED_OFF ) {
					// disable led
					digitalWrite(pin_green, LOW);
		    		digitalWrite(pin_red, LOW);
				}

		previous_state = state;
	}; 
};

#endif // __LED_H__
