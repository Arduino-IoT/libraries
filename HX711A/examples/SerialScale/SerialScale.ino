/* sample for digital weight scale of hx711, display with a HD44780 liquid crtstal monitor
 *
 * hardware design: syyyd
 * available at http://syyyd.taobao.com
 *
 * library design: Weihong Guan (@aguegu)
 * http://aguegu.net
 *
 * library host on
 * https://github.com/aguegu/Arduino
 */

// Hx711.DOUT - pin 9
// Hx711.SCK - pin 10


#include <Hx711.h>


Hx711 scale(9,10);

void setup() {
  Serial.begin(9600);
  scale.setScale(705.0f);
}

void loop() {

  double weigth = scale.getGram();
  
  Serial.println(weigth,1);
 

  delay(200);
}
