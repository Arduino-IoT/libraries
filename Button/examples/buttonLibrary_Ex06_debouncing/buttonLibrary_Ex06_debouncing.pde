 /*
 * Button Library Example 6 -  information about debouncing
 * ------------
 *
 * Circuit: 
 *          - Button, pulled up externally with 10k resitor on pin 2
 *          - Extra LED on pin 9
 *
 * Behavior: 
 *         LEDs depend on a button's press state using if statement
 *         When button is pressed the LED on 9 is on.
 *         When the button is released the LED on pin 13 is on
 *
 *         The startup loop prints out the default value of the 
 *         debounce delay (30 millis) and then updates it with the 
 *         value in newDebounceDelay. And then, just for giggles,
 *         wipes it out entirely
 *
 *
 * Created 13 April 2007
 * Updated 29 October 2010
 * by Carlyn Maw
 *
 */

#include <Button.h>

int ledPin1 = 9;                 
int ledPin2 = 13;               




//Instantiate Button on digital pin 2
//pressed = ground (i.e. pulled high with external resistor)
Button helloButton = Button(2, LOW);
int newDebounceDelay = 50;              //30 is the default

void setup()
{

  Serial.begin(9600);
  Serial.print("Default = \t");
  Serial.println(helloButton.getDebounceDelay());
  helloButton.setDebounceDelay(newDebounceDelay);
  Serial.print("New Delay = \t");
  Serial.println(helloButton.getDebounceDelay());
  helloButton.clearDebounceDelay();
  Serial.print("New Delay = \t");
  Serial.println(helloButton.getDebounceDelay());



}

void loop()
{

  helloButton.listen();  


  if (helloButton.isPressed()) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, LOW);
  }


  if (helloButton.isReleased()) {
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin1, LOW);

  }

}

