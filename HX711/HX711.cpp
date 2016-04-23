#include <Arduino.h>
#include <HX711.h>

HX711::HX711(byte sck, byte dout, byte amp, double co) {
    SCK = sck;
    DOUT = dout;
    set_amp(amp);
    COEFFICIENT = co;
    pinMode(SCK, OUTPUT);
    pinMode(DOUT, INPUT);
    digitalWrite(SCK, LOW);
    read();
}

void HX711::set_amp(byte amp) {
    switch (amp) {
        case 32: AMP = 2; break;
        case 64: AMP = 3; break;
        case 128: AMP = 1; break;
    }
}

bool HX711::is_ready() {
    return digitalRead(DOUT) == LOW;
}

long HX711::read() {
    long val = 0;
    while (!is_ready());
    for (int i = 0; i < 24; i++) {
        pulse(SCK);
        val <<= 1;
        if (digitalRead(DOUT) == HIGH) val++;
    }
    for (int i = 0; i < AMP; i++) {
        pulse(SCK);
    }
    return val & (1L << 23) ? val | ((-1L) << 24) : val;
}

double HX711::bias_read() {
    return (read() - OFFSET) * COEFFICIENT;
}

void HX711::tare(int t) {
    double sum = 0;
    for (int i = 0; i < t; i++) {
        sum += read();
    }
    set_offset(sum / t);
}

void HX711::set_offset(long offset) {
    OFFSET = offset;
}

void HX711::set_co(double co) {
    COEFFICIENT = co;
}
