Four Digit Display Suli
-------------------------------------------------------------
![image](http://www.seeedstudio.com/depot/images/product/4-Digital%20Display.jpg)

4 digit display module is usually a 12 pin module. In this Grove gadget, we utilize a TM1637 to scale down the controlling pins into 2 Grove pins. It only takes 2 digital pins of Arduino or Seeeduino to control the content, even the luminance of this display. For projects that require of alpha-numeric display, this can be a nice choice.
 
**Features**

* 4 digit red alpha-numeric display
* Grove compatible
* 8 adjustable luminance levels


**Application Ideas**

* Time display
* Stopwatch
* Sensors’ input display



#Usage

**Initialize**

This function use to initialize the device at the very beginning. You can set the pin to used here. 

	void four_digit_init(PIN_T data, PIN_T clk);


**Set Brightness**

This function can set the brightness of the device. 

	void four_digit_set_brightness(enum brightness_t b);


And the enum of brightness was defined in Four_Digit_Display.h

	enum brightness_t
	{
	    BRIGHT_DARKEST = 0,
	    BRIGHT_TYPICAL = 2,
	    BRIGHTEST      = 7
	};


**Set Point**

You can find a colon in the middle of the display. This function can turn on and off this colon.

	void four_digit_set_point(bool disp_point);		// 1-on, 0-off


**Display a Bit**

You can find 4Bit in the display. 

	void four_digit_display(uint8 BitAddr,int8 DispData);


BitAddr: 0-3, from right to left
DispData: number to display, should be between 0-9


**Clear**

Clear all the display. 

	void four_digit_clear();

<br>
-----
**Note:**

This is a Suli-Compatible Library, You Should Download Suli for your platform, please refer to [https://github.com/Seeed-Studio/Suli](https://github.com/Seeed-Studio/Suli "Suli")

If you can't find Suli for your platform, please contract us, If you can port it yourself and pull request to us, We'll be appreciated. 

***For Arduino:***

Just like you use the other Library, What's different is you should download Suli for Arduino First, remember that just put Suli for Arduino to your Arduino libraries folder.

***For non-Arduino Platform:***

For a non-Arduino platform, what you need is xxx.h and xxx.cpp(you can change it to xxx.c while your compiler can't support c++), the other files like xxx_Arduino.h and examples is for Arduino, you can just delete them.

<br>
----
This software is written by Loovee for seeed studio<br>
and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>


[1]:http://www.seeedstudio.com/wiki/LED_Bar



[![Analytics](https://ga-beacon.appspot.com/UA-46589105-3/Grove_LED_Bar)](https://github.com/igrigorik/ga-beacon)
