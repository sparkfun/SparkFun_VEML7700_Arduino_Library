/*!
 * @file Example2_changeSettings.ino
 *
 * This example was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 * 
 * This example demonstrates how to change the VEML7700's sensitivity (gain) and integration time settings.
 * 
 * Want to support open source hardware? Buy a board from SparkFun!
 * <br>SparkX smôl Environmental Peripheral Board (SPX-18976): https://www.sparkfun.com/products/18976
 * 
 * Please see LICENSE.md for the license information
 * 
 */

#include <SparkFun_VEML7700_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_VEML7700

VEML7700 mySensor; // Create a VEML7700 object

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

  //The default integration time is 100ms.
  //Possible values are:
  //VEML7700_INTEGRATION_25ms
  //VEML7700_INTEGRATION_50ms
  //VEML7700_INTEGRATION_100ms
  //VEML7700_INTEGRATION_200ms
  //VEML7700_INTEGRATION_400ms
  //VEML7700_INTEGRATION_800ms
  //Let's change the integration time to 50ms:
  mySensor.setIntegrationTime(VEML7700_INTEGRATION_50ms);

  //Confirm the integration time was set correctly
  Serial.print(F("The sensor integration time is: "));
  Serial.println(mySensor.getIntegrationTimeStr());

  //The default gain (sensitivity mode) is x1
  //Possible values are:
  //VEML7700_SENSITIVITY_x1
  //VEML7700_SENSITIVITY_x2
  //VEML7700_SENSITIVITY_x1_8
  //VEML7700_SENSITIVITY_x1_4
  //Let's change the sensitivity to x2:
  mySensor.setSensitivityMode(VEML7700_SENSITIVITY_x2);

  //Confirm that the sensitivity mode was set correctly
  Serial.print(F("The sensor gain (sensitivity mode) is: "));
  Serial.println(mySensor.getSensitivityModeStr());

  //We can also change the persistence ptotect number. Default is 1.
  //Possible values are:
  //VEML7700_PERSISTENCE_1
  //VEML7700_PERSISTENCE_2
  //VEML7700_PERSISTENCE_4
  //VEML7700_PERSISTENCE_8
  //Let's change the persistence protect to 4:
  mySensor.setPersistenceProtect(VEML7700_PERSISTENCE_4);

  //Confirm that the persistence protect was set correctly
  Serial.print(F("The sensor persistence protect setting is: "));
  Serial.println(mySensor.getPersistenceProtectStr());  

  Serial.println(F("Lux:\tAmbient:\tWhite Level:"));
}

void loop()
{
  Serial.print(mySensor.getLux(), 4); // Read the lux from the sensor and print it
  Serial.print("\t");
  Serial.print(mySensor.getAmbientLight()); // Read the ambient light level
  Serial.print("\t");
  Serial.println(mySensor.getWhiteLevel()); // Read the white channel level
  delay(250);
}
