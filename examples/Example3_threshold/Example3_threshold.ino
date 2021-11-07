/*!
 * @file Example3_threshold.ino
 *
 * This example was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 * 
 * This example demonstrates how to change the VEML7700's threshold settings.
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

  //Let's change the high threshold to 30000 counts:
  mySensor.setHighThreshold(30000);

  //Confirm the high threshold was set correctly
  Serial.print(F("The high threshold is: "));
  Serial.println(mySensor.getHighThreshold());

  //Let's change the low threshold to 1000 counts:
  mySensor.setLowThreshold(1000);

  //Confirm the low threshold was set correctly
  Serial.print(F("The low threshold is: "));
  Serial.println(mySensor.getLowThreshold());

  //Enable the high and low threshold interrupts
  mySensor.setInterruptEnable(VEML7700_INT_ENABLE);

  //Check that the interrupts are enabled
  Serial.print(F("Interrupts are "));
  VEML7700_interrupt_enable_t ie;
  mySensor.getInterruptEnable(&ie);
  if ((ie == VEML7700_INT_DISABLE) || ( ie == VEML7700_INT_INVALID))
    Serial.print(F("not "));
  Serial.println(F("enabled"));
}

void loop()
{
  Serial.print(F("Ambient: "));
  Serial.print(mySensor.getAmbientLight()); // Read the ambient light level from the sensor and print it

  // Note: reading the interrupt status register clears the interrupts, so we need to check both
  //       the high and low interrupt flags in a single read
  
  VEML7700_interrupt_status_t intStatus = mySensor.getInterruptStatus(); // Check the interrupt status

  // Possible values for intStatus are:
  // VEML7700_INT_STATUS_NONE
  // VEML7700_INT_STATUS_HIGH
  // VEML7700_INT_STATUS_LOW
  // VEML7700_INT_STATUS_BOTH
  // VEML7700_INT_STATUS_INVALID

  if (intStatus == VEML7700_INT_STATUS_INVALID)
  {
    Serial.print(F("\tInterrupt Status Read Error!"));
  }
  else
  {
    if (intStatus & VEML7700_INT_STATUS_HIGH) // Use a logical AND to check if the high flag is set
      Serial.print(F("\tHigh Threshold Exceeded"));

    if (intStatus & VEML7700_INT_STATUS_LOW) // Use a logical AND to check if the low flag is set
      Serial.print(F("\tLow Threshold Exceeded"));
  }

  Serial.println();

  delay(250);
}
