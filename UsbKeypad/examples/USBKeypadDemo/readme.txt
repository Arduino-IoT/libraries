In order to run USbKeypad with Arduino, you need to modify Arduino core,
change Timer0 ISR with a "sei()" call as the first instruction, e.g:

SIGNAL(TIMER0_OVF_vect)
{
    sei();             // ensure that the USB interrupt is not disabled
                       // for ore than 25 cycles(@12MHz).
                       // this prevents V-USB from malfunction.
    ...
}