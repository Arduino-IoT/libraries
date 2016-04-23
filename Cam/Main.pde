Example sketch
-----------------
#include <Fat16.h>
#include <EEPROM.h>
#include "CAM.h"

CAM camera = CAM();

void setup()
{  
	camera.setup("VGA", 115200);
}
void loop()
{
	camera.shoot(gps.Time, gps.Latitude, gps.Longitude, gps.Altitude);
}