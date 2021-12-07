/*!
 * @file Example6_ProductionTest.ino
 *
 * This example was written by:
 * Paul Clark
 * SparkFun Electronics
 * December 7th 2021
 * 
 * This is the code we use to test the SPX-18981 Ambient Light Sensor.
 * LED_BUILTIN will light up if the VEML7700 is detected correctly and the lux is within bounds.
 * 
 * Want to support open source hardware? Buy a board from SparkFun!
 * <br>SparkX Ambient Light Sensor - VEML7700 (SPX-18981): https://www.sparkfun.com/products/18981
 * 
 * Please see LICENSE.md for the license information
 * 
 */

#include <SparkFun_VEML7700_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_VEML7700

VEML7700 mySensor; // Create a VEML7700 object

void setup()
{
  Serial.begin(115200);
  Serial.println(F("VEML7700 Production Test"));

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  delay(250); // Try every 0.25 seconds

  Wire.begin();

  Wire.beginTransmission(0x10); // Detect VEML7700 on address 0x10
  if (Wire.endTransmission() != 0)
  {
    digitalWrite(LED_BUILTIN, LOW);
    Wire.end();
    Serial.println(F("Error: nothing detected on address 0x10"));
    return;
  }
  
  //Initialize sensor
  if (mySensor.begin() == false)
  {
    digitalWrite(LED_BUILTIN, LOW);
    Wire.end();
    Serial.println(F("Error: .begin failed"));
    return;
  }

  delay(250); // Default integration time is 100ms

  // Now we read the lux from the sensor
  float lux = mySensor.getLux(); // Read the lux

  // Check the lux is within bounds
  if ((lux < 100.0) || (lux > 100000.0))
  {
    digitalWrite(LED_BUILTIN, LOW);
    Wire.end();
    Serial.print(F("Error: invalid lux reading: "));
    Serial.println(lux, 4);
    return;
  }

  // All good!
  Serial.println(F("Test passed!"));
  digitalWrite(LED_BUILTIN, HIGH);
  Wire.end();
}
