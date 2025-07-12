/*!
 * @file Example1_getLux.ino
 *
 * This example was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 *
 * This example demonstrates how to initialize the VEML7700 and then get the ambient light lux.
 *
 * Want to support open source hardware? Buy a board from SparkFun!
 * <br>SparkX smôl Environmental Peripheral Board (SPX-18976): https://www.sparkfun.com/products/18976
 *
 * Please see LICENSE.md for the license information
 *
 */

#include <SparkFun_VEML7700_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_VEML7700

SparkFunVEML7700 mySensor; // Create a VEML7700 object

void setup()
{
    Serial.begin(115200);
    Serial.println(F("SparkFun VEML7700 Example"));

    Wire.begin();

    // mySensor.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

    // Begin the VEML7700 using the Wire I2C port
    // .begin will return true on success, or false on failure to communicate
    if (mySensor.begin() == false)
    {
        Serial.println("Unable to communicate with the VEML7700. Please check the wiring. Freezing...");
        while (1)
            ;
    }

    Serial.println(F("Lux:"));
}

void loop()
{
    Serial.println(mySensor.getLux(), 4); // Read the lux from the sensor and print it
    delay(250);
}
