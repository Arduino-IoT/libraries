#include <HX711.h>

HX711 hx(9, 10);

void setup() {
  Serial.begin(9600);
}

void loop()
{
  double sum = 0;
  for (int i = 0; i < 10; i++)
    sum += hx.read();
  Serial.println(sum/10);
}
