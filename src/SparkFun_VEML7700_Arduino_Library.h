/******************************************************************************
 * @file SparkFun_VEML7700_Arduino_Library.h
 * @brief SparkFun VEML7700 Library header file
 *
 * This file implements the SparkFunVEML7700 class
 * for use with the SparkFun VEML7700 sensor qwiic breakout board
 *
 * @author SparkFUn Electronics
 * @date 2021-2025
 * @version 2.0.0
 * @copyright (c) 2021-2025 SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 *
 ******************************************************************************/

#pragma once

// helps to keep the Toolkit header before the tk calls
// clang-format off
#include <SparkFun_Toolkit.h>
#include "sfTk/sfDevVEML7700.h"
// clang-format on

// For backwards compatibility - with the SparkX (v1.0.0) library, remap error codes
// to the toolkit error codes
typedef enum : sfTkError_t
{
    VEML7700_ERROR_READ = ksfTkErrFail,
    VEML7700_ERROR_WRITE = ksfTkErrFail,
    VEML7700_ERROR_INVALID_ADDRESS = ksfTkErrInvalidParam,
    VEML7700_ERROR_UNDEFINED = ksfTkErrInvalidParam,
    VEML7700_ERROR_SUCCESS = ksfTkErrOk
} VEML7700_error_t;
const VEML7700_error_t VEML7700_SUCCESS = VEML7700_ERROR_SUCCESS;
/**
 * @brief Class for interfacing with the VMEL7700 sensor using I2C communication
 *
 * This class provides methods to initialize and communicate with the BMV080 sensor
 * over an I2C bus. It inherits from the sfDevVEML7700 class and uses the SparkFun
 * Toolkit for I2C communication.
 *
 * @see sfDevVEML7700
 */
class SparkFunVEML7700 : public sfDevVEML7700
{
  public:
    /**
     * @brief Begins the Device with I2C as the communication bus
     *
     * This method initializes the I2C bus and sets up communication with the VEML7700 sensor.
     *
     * @param address I2C device address to use for the sensor
     * @param wirePort Wire port to use for I2C communication
     * @return True if successful, false otherwise
     */
    bool begin(const uint8_t address = SF_VEML7700_DEFAULT_ADDRESS, TwoWire &wirePort = Wire)
    {
        // Setup Arduino I2C bus
        _theI2CBus.init(wirePort, address);
        _theI2CBus.setStop(false); // Do not send a stop condition after each read/write

        // Begin the sensor
        sfTkError_t rc = sfDevVEML7700::begin(&_theI2CBus);

        return rc == ksfTkErrOk ? isConnected() : false;
    }

  private:
    sfTkArdI2C _theI2CBus;
};

// for backwards compatibility
class VEML7700 : public SparkFunVEML7700
{
};