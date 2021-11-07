/*!
 * @file Example5_alternatePorts.ino
 *
 * This example was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 * 
 * This example demonstrates how to use alternate Wire and Serial ports when communicating with the VEML7700.
 * 
 * Want to support open source hardware? Buy a board from SparkFun!
 * <br>SparkX smôl Environmental Peripheral Board (SPX-18976): https://www.sparkfun.com/products/18976
 * 
 * Please see LICENSE.md for the license information
 * 
 */

#include <SparkFun_VEML7700_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_VEML7700

VEML7700 mySensor; // Create a VEML7700 object

#define myWire Wire // Change this to (e.g.) Wire1 if required
#define mySerial Serial // Change this to (e.g.) Serial1 if required

void setup()
{
  mySerial.begin(115200);
  mySerial.println(F("SparkFun VEML7700 Example"));

  myWire.begin();

  //mySensor.enableDebugging(mySerial); // Uncomment this line to enable helpful debug messages on mySerial

  // Begin the VEML7700 using the myWire I2C port
  // .begin will return true on success, or false on failure to communicate
  if (mySensor.begin(myWire) == false)
  {
    mySerial.println("Unable to communicate with the VEML7700. Please check the wiring. Freezing...");
    while (1)
      ;
  }

  mySerial.println(F("Lux:"));
}

void loop()
{
  mySerial.println(mySensor.getLux(), 4); // Read the lux from the sensor and print it
  delay(250);
}
