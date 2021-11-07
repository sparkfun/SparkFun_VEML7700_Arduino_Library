/*!
 * @file Example4_shutDown.ino
 *
 * This example was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 * 
 * This example demonstrates how to shut down the VEML7700.
 * 
 * Want to support open source hardware? Buy a board from SparkFun!
 * <br>SparkX smôl Environmental Peripheral Board (SPX-18976): https://www.sparkfun.com/products/18976
 * 
 * Please see LICENSE.md for the license information
 * 
 */

#include <SparkFun_VEML7700_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_VEML7700

VEML7700 mySensor; // Create a VEML7700 object

unsigned long lastMillis = 0; // Keep track of time

VEML7700_shutdown_t shutdownState = VEML7700_POWER_ON; // Toggle between VEML7700_POWER_ON and VEML7700_SHUT_DOWN

void setup()
{
  Serial.begin(115200);
  Serial.println(F("SparkFun VEML7700 Example"));

  Wire.begin();

  //mySensor.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  // Begin the VEML7700 using the Wire I2C port
  // .begin will return true on success, or false on failure to communicate
  if (mySensor.begin() == false)
  {
    Serial.println("Unable to communicate with the VEML7700. Please check the wiring. Freezing...");
    while (1)
      ;
  }

  lastMillis = millis(); // Keep track of time
}

void loop()
{
  if (shutdownState == VEML7700_POWER_ON) // Are we "On"? (Comment this line if you are interested in how it effects the sleep current)
  {
    Serial.print(F("Lux: "));
    Serial.println(mySensor.getLux(), 4); // Read the lux from the sensor and print it
  }

  //Check if it is time to change the power state
  if (millis() > (lastMillis + 5000)) // Change state every 5 seconds
  {
    lastMillis = millis(); // Keep track of time

    if (shutdownState == VEML7700_POWER_ON) // Are we "On"?
    {
      shutdownState = VEML7700_SHUT_DOWN; // Put sensor to sleep
      mySensor.setShutdown(shutdownState);
      
      //mySensor.shutDown(); // This would do the same thing
    }
    else
    {
      shutdownState = VEML7700_POWER_ON; // Power sensor on
      mySensor.setShutdown(shutdownState);
      
      //mySensor.powerOn(); // This would do the same thing
    }
  }

  delay(125);
}
