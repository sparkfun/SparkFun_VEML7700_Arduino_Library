/*!
 * @file sfDevVEML7700.cpp
 *
 * @mainpage SparkFun VEML7700 Ambient Light Sensor Arduino Library
 *
 * @section intro_sec Introduction
 *
 * This library facilitates communication with the VEML7700 over I<sup>2</sup>C.
 *
 * Want to support open source hardware? Buy a board from SparkFun!
 *
 * @section author Author
 *
 * This library was written by:
 * Paul Clark
 * SparkFun Electronics
 * November 4th 2021
 *
 * @author SparkFun Electronics
 * @date 2021-2025
 * @copyright Copyright (c) 2021-2025, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT

 *
 */

#include "sfDevVEML7700.h"
#include "Arduino.h"

/**
 * @brief Lookup table for VEML7700 lux resolution values.
 *
 * This 2D array provides the lux resolution (in lux/count) for different gain (sensitivity) and integration time
 * settings of the VEML7700 ambient light sensor. The first dimension corresponds to the gain setting, and the second
 * dimension corresponds to the integration time setting.
 *
 * Gain (sensitivity) indices:
 *   0: Gain 1
 *   1: Gain 2
 *   2: Gain 1/8
 *   3: Gain 1/4
 *
 * Integration time indices:
 *   0: 25ms
 *   1: 50ms
 *   2: 100ms
 *   3: 200ms
 *   4: 400ms
 *   5: 800ms
 *
 * Each element [gain][integration_time] gives the corresponding lux resolution for that configuration.
 */
const float VEML7700_LUX_RESOLUTION[VEML7700_SENSITIVITY_INVALID][VEML7700_INTEGRATION_INVALID] = {
    // 25ms    50ms    100ms   200ms   400ms   800ms
    {0.2304, 0.1152, 0.0576, 0.0288, 0.0144, 0.0072}, // Gain (sensitivity) 1
    {0.1152, 0.0576, 0.0288, 0.0144, 0.0072, 0.0036}, // Gain (sensitivity) 2
    {1.8432, 0.9216, 0.4608, 0.2304, 0.1152, 0.0576}, // Gain (sensitivity) 1/8
    {0.9216, 0.4608, 0.2304, 0.1152, 0.0576, 0.0288}  // Gain (sensitivity) 1/4
};

/**
 * @brief Array of string representations for VEML7700 gain (sensitivity) settings.
 *
 * This array maps each gain setting constant to its corresponding string value:
 * - "x1"    : Gain setting x1
 * - "x2"    : Gain setting x2
 * - "x1/8"  : Gain setting x1/8
 * - "x1/4"  : Gain setting x1/4
 * - "INVALID": Invalid gain setting
 *
 * The array size is determined by VEML7700_SENSITIVITY_INVALID + 1 to include all possible gain settings.
 *
 * @note The array is indexed by the VEML7700_sensitivity_mode_t enum values.
 */
const char *kVEML7700GainSettingsString[VEML7700_SENSITIVITY_INVALID + 1] = {"x1", "x2", "x1/8", "x1/4", "INVALID"};

/**
 * @brief Array of string representations for VEML7700 sensor integration times.
 *
 * Each element corresponds to a specific integration time setting for the VEML7700 sensor.
 * The last element "INVALID" represents an invalid or unsupported integration time.
 *
 * Index mapping:
 *   0 - "25ms"
 *   1 - "50ms"
 *   2 - "100ms"
 *   3 - "200ms"
 *   4 - "400ms"
 *   5 - "800ms"
 *   6 - "INVALID"
 */
const char *kVEML7799IntegrationTimesString[VEML7700_INTEGRATION_INVALID + 1] = {"25ms",  "50ms",  "100ms",  "200ms",
                                                                                 "400ms", "800ms", "INVALID"};

/**
 * @brief String representations for VEML7700 persistence protection settings.
 *
 * This array maps the persistence protection enumeration values to their corresponding string representations.
 * The strings represent the number of persistence events required before an interrupt is triggered.
 * The last entry "INVALID" corresponds to an invalid persistence setting.
 *
 * Index mapping:
 *   0: "1"      - 1 persistence event
 *   1: "2"      - 2 persistence events
 *   2: "4"      - 4 persistence events
 *   3: "8"      - 8 persistence events
 *   4: "INVALID" - Invalid persistence setting
 */
const char *kVEML7700PersistenceProtectStrings[VEML7700_PERSISTENCE_INVALID + 1] = {"1", "2", "4", "8", "INVALID"};

//--------------------------------------------------------------------------------------------------
// Helpful method - used by a majority of methods - reads the configuration register
sfTkError_t sfDevVEML7700::updateConfiguration(sfDevVEML7700Config_t &config)
{
    if (_theBus == nullptr)
        return ksfTkErrBusNotInit; // Not initialized

    return _theBus->readRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}
//--------------------------------------------------------------------------------------------------
// Begin the VEML7700 device. Requires a bus object to communicate with the device.
//
sfTkError_t sfDevVEML7700::begin(sfTkIBus *theBus)
{
    // Nullptr check
    if (theBus == nullptr)
        return ksfTkErrBusNotInit;

    // Set bus pointer
    _theBus = theBus;

    /** Write _configurationRegister into the VEML7700_CONFIGURATION_REGISTER.
        This will place the device into a known state, in case it was configured previously
        and remained powered on when the code was restarted. */
    sfDevVEML7700Config_t configurationRegister;
    configurationRegister.all = 0x0000; // Clear the reserved bits
    configurationRegister.CONFIG_REG_SD = VEML7700_POWER_ON;
    configurationRegister.CONFIG_REG_INT_EN = VEML7700_INT_DISABLE;
    configurationRegister.CONFIG_REG_PERS = VEML7700_PERSISTENCE_1;
    configurationRegister.CONFIG_REG_IT = (VEML7700_t)integrationTimeConfig(VEML7700_INTEGRATION_100ms);
    configurationRegister.CONFIG_REG_SM = VEML7700_SENSITIVITY_x1;

    return theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, configurationRegister.all);
}

//--------------------------------------------------------------------------------------------------
// Check if the VEML7700 is connected by reading the configuration register.
//
bool sfDevVEML7700::isConnected(void)
{
    sfDevVEML7700Config_t config;
    return updateConfiguration(config) == ksfTkErrOk;
}

//--------------------------------------------------------------------------------------------------
// Set the VEML7700's shut down setting. This will power down the device if shutdown is true
//
sfTkError_t sfDevVEML7700::setShutdown(bool shutdown)
{

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the configuration register

    config.CONFIG_REG_SD = shutdown ? VEML7700_SHUT_DOWN : VEML7700_POWER_ON;

    return _theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}

//--------------------------------------------------------------------------------------------------
// Is the VEML7700 in shut down mode?
// Return true if the device is in shut down mode, false otherwise.
//
bool sfDevVEML7700::isShutdown(void)
{
    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return true; // Error reading the configuration register

    return (((VEML7700_shutdown_t)config.CONFIG_REG_SD) == VEML7700_SHUT_DOWN);
}

//--------------------------------------------------------------------------------------------------
// Set the VEML7700's interrupt enable setting.
//
sfTkError_t sfDevVEML7700::enableInterrupt(bool bEnable)
{

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the configuration register

    config.CONFIG_REG_INT_EN = (VEML7700_t)bEnable;

    return _theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}
//--------------------------------------------------------------------------------------------------
// Get the interrupt enable setting.
//

bool sfDevVEML7700::isInterruptEnabled(void)
{

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return false; // Error reading the configuration register

    return config.CONFIG_REG_INT_EN == VEML7700_INT_ENABLE;
}

//--------------------------------------------------------------------------------------------------
// Set the VEML7700's Persistence Protect Number setting
//

sfTkError_t sfDevVEML7700::setPersistenceProtect(VEML7700_persistence_protect_t pp)
{
    if (pp >= VEML7700_PERSISTENCE_INVALID)
        return ksfTkErrInvalidParam;

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the configuration register

    config.CONFIG_REG_PERS = (VEML7700_t)pp;
    return _theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}
//--------------------------------------------------------------------------------------------------
// Get the VEML7700's Persistence Protect Number setting
// Method provides an error code as well as the value of the persistence protect number
//

sfTkError_t sfDevVEML7700::getPersistenceProtect(VEML7700_persistence_protect_t &pp)
{
    pp = VEML7700_PERSISTENCE_INVALID; // Default to invalid

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc == ksfTkErrOk)
        pp = (VEML7700_persistence_protect_t)config.CONFIG_REG_PERS;

    return rc;
}

//--------------------------------------------------------------------------------------------------
// Get the VEML7700's Persistence Protect Number setting
//
VEML7700_persistence_protect_t sfDevVEML7700::persistenceProtect(void)
{
    VEML7700_persistence_protect_t pp;
    sfTkError_t rc = getPersistenceProtect(pp);
    return rc == ksfTkErrOk ? pp : VEML7700_PERSISTENCE_INVALID;
}
//--------------------------------------------------------------------------------------------------
// Return the string representation of the VEML7700's Persistence Protect Number setting
//
const char *sfDevVEML7700::persistenceProtectString(void)
{
    VEML7700_persistence_protect_t pp = persistenceProtect();

    if (pp < VEML7700_PERSISTENCE_1 || pp >= VEML7700_PERSISTENCE_INVALID)
        return "INVALID"; // Invalid persistence protect setting

    return kVEML7700PersistenceProtectStrings[pp];
}

//--------------------------------------------------------------------------------------------------
// Set the VEML7700's integration time setting
//
sfTkError_t sfDevVEML7700::setIntegrationTime(VEML7700_integration_time_t it)
{

    if (it >= VEML7700_INTEGRATION_INVALID)
        return ksfTkErrInvalidParam; // Invalid integration time setting

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the configuration register

    config.CONFIG_REG_IT = (VEML7700_t)integrationTimeConfig(it);
    return _theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}

//--------------------------------------------------------------------------------------------------
// Get the VEML7700's integration time setting - returning an error code as well as the value
//
sfTkError_t sfDevVEML7700::getIntegrationTime(VEML7700_integration_time_t &it)
{
    it = VEML7700_INTEGRATION_INVALID; // Default to invalid

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc == ksfTkErrOk)
        it = integrationTimeFromConfig((VEML7700_config_integration_time_t)config.CONFIG_REG_IT);

    return rc;
}

//--------------------------------------------------------------------------------------------------
// Get the VEML7700's integration time setting
//
VEML7700_integration_time_t sfDevVEML7700::integrationTime(void)
{
    VEML7700_integration_time_t it;

    sfTkError_t rc = getIntegrationTime(it);

    return rc == ksfTkErrOk ? it : VEML7700_INTEGRATION_INVALID; // Return invalid on error
}
//--------------------------------------------------------------------------------------------------
// Get the VEML7700's integration time setting as a string
//
const char *sfDevVEML7700::integrationTimeString(void)
{
    VEML7700_integration_time_t it = integrationTime();

    if (it < VEML7700_INTEGRATION_25ms || it >= VEML7700_INTEGRATION_INVALID)
        return "INVALID"; // Invalid integration time setting

    return kVEML7799IntegrationTimesString[it];
}

//--------------------------------------------------------------------------------------------------
// Set the VEML7700's sensitivity mode setting (ALS_SM)
//
sfTkError_t sfDevVEML7700::setSensitivityMode(VEML7700_sensitivity_mode_t sm)
{
    if (sm >= VEML7700_SENSITIVITY_INVALID)
        return ksfTkErrInvalidParam;

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the configuration register

    config.CONFIG_REG_SM = (VEML7700_t)sm;
    return _theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}

//--------------------------------------------------------------------------------------------------
// Get the VEML7700's sensitivity mode setting (ALS_SM) - returning an error code as well as the value
//
sfTkError_t sfDevVEML7700::getSensitivityMode(VEML7700_sensitivity_mode_t &sm)
{
    sm = VEML7700_SENSITIVITY_INVALID; // Default to invalid
    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc == ksfTkErrOk)
        sm = (VEML7700_sensitivity_mode_t)config.CONFIG_REG_SM;

    return rc;
}

//--------------------------------------------------------------------------------------------------
// Get the VEML7700's sensitivity mode setting (ALS_SM)
VEML7700_sensitivity_mode_t sfDevVEML7700::sensitivityMode()
{
    VEML7700_sensitivity_mode_t sm;

    sfTkError_t rc = getSensitivityMode(sm);

    return rc == ksfTkErrOk ? sm : VEML7700_SENSITIVITY_INVALID; // Return invalid on error
}

//--------------------------------------------------------------------------------------------------
// Get the VEML7700's sensitivity mode setting (ALS_SM) as a string
//

const char *sfDevVEML7700::sensitivityModeString()
{
    VEML7700_sensitivity_mode_t sm = sensitivityMode();

    if (sm < VEML7700_SENSITIVITY_x1 || sm >= VEML7700_SENSITIVITY_INVALID)
        return "INVALID"; // Invalid sensitivity mode setting

    return kVEML7700GainSettingsString[sm];
}

//--------------------------------------------------------------------------------------------------
// Set the VEML7700's ALS high threshold window setting (ALS_WH)
//
sfTkError_t sfDevVEML7700::setHighThreshold(uint16_t threshold)
{
    return _theBus->writeRegister(VEML7700_HIGH_THRESHOLD, threshold);
}

//--------------------------------------------------------------------------------------------------
// Get the VEML7700's ALS high threshold window setting (ALS_WH) - returning an error code as well as the value
//

sfTkError_t sfDevVEML7700::getHighThreshold(uint16_t &threshold)
{
    return _theBus->readRegister(VEML7700_HIGH_THRESHOLD, threshold);
}
//--------------------------------------------------------------------------------------------------
// Get the VEML7700's ALS high threshold window setting (ALS_WH)
//
uint16_t sfDevVEML7700::highThreshold(void)
{
    uint16_t threshold;
    sfTkError_t rc = getHighThreshold(threshold);

    return rc == ksfTkErrOk ? threshold : kVEML7700ValueError; // Return 0xFFFF on error
}

//---------------------------------------------------------------------------------------------------
// Set the VEML7700's ALS low threshold window setting (ALS_WL)
//
sfTkError_t sfDevVEML7700::setLowThreshold(uint16_t threshold)
{
    return _theBus->writeRegister(VEML7700_LOW_THRESHOLD, threshold);
}

//---------------------------------------------------------------------------------------------------
// Get the VEML7700's ALS low threshold window setting (ALS_WL) - returning an error code as well as the value
//
sfTkError_t sfDevVEML7700::getLowThreshold(uint16_t &threshold)
{
    return _theBus->readRegister(VEML7700_LOW_THRESHOLD, threshold);
}

//---------------------------------------------------------------------------------------------------
// Get the VEML7700's ALS low threshold window setting (ALS_WL)
//
uint16_t sfDevVEML7700::lowThreshold(void)
{
    uint16_t threshold;
    sfTkError_t rc = getLowThreshold(threshold);

    return rc == ksfTkErrOk ? threshold : kVEML7700ValueError; // Return 0xFFFF on error
}

//----------------------------------------------------------------------------------------------------
// Read the ambient ligth sensor data (ALS) - also return an error code
//
sfTkError_t sfDevVEML7700::getAmbientLight(uint16_t &ambient)
{
    return _theBus->readRegister(VEML7700_ALS_OUTPUT, ambient);
}

//----------------------------------------------------------------------------------------------------
// Read the ambient light sensor data (ALS)
//
uint16_t sfDevVEML7700::getAmbientLight(void)
{

    uint16_t ambient;
    sfTkError_t rc = getAmbientLight(ambient);

    return rc == ksfTkErrOk ? ambient : kVEML7700ValueError;
}

//----------------------------------------------------------------------------------------------------
// Read the VEML7700's white level data (WHITE) - also return an error code
//
sfTkError_t sfDevVEML7700::getWhiteLevel(uint16_t &whiteLevel)
{
    return _theBus->readRegister(VEML7700_WHITE_OUTPUT, whiteLevel);
}

//----------------------------------------------------------------------------------------------------
// Read the VEML7700's white level data (WHITE)
//
uint16_t sfDevVEML7700::getWhiteLevel(void)
{
    uint16_t whiteLevel;
    sfTkError_t rc = getWhiteLevel(whiteLevel);

    return rc == ksfTkErrOk ? whiteLevel : kVEML7700ValueError;
}

//----------------------------------------------------------------------------------------------------
// Calculate the lux from the ambient light sensor data (ALS)
// This method also returns an error code
//
sfTkError_t sfDevVEML7700::getLux(float &lux)

{
    lux = 0.0f; // Default to 0.0f

    // First, we need to extract the correct resolution from the VEML7700_LUX_RESOLUTION
    // gain and integration time look up table. Let's begin by reading the gain
    // (sensitivity) and integration time.

    sfTkError_t rc;

    VEML7700_sensitivity_mode_t senseMode;
    rc = getSensitivityMode(senseMode);

    if (rc != ksfTkErrOk)
        return rc;

    VEML7700_integration_time_t intTime;
    rc = getIntegrationTime(intTime);

    if (rc != ksfTkErrOk)
        return rc;

    // Now we read the ambient level and multiply it by the resolution
    uint16_t ambient;
    rc = getAmbientLight(ambient);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the ambient light

    // Apply the resolution to the ambient light reading
    lux = ambient * VEML7700_LUX_RESOLUTION[senseMode][intTime];

    return ksfTkErrOk; // Return success
}

//----------------------------------------------------------------------------------------------------
// Read the lux from the ambient light sensor data (ALS)
//
float sfDevVEML7700::getLux(void)
{
    float lux;
    sfTkError_t rc = getLux(lux);

    return rc == ksfTkErrOk ? lux : kVEML7700ValueError; // Return 0.0f on error
}

//----------------------------------------------------------------------------------------------------
// Read the VEML7700's interrupt status register
//
// Note: reading the interrupt status register clears the interrupts.
//       So, we need to check both interrupt flags in a single read.
//
VEML7700_interrupt_status_t sfDevVEML7700::interruptStatus(void)
{
    VEML7700_INTERRUPT_STATUS_REGISTER_t isr;

    sfTkError_t rc = _theBus->readRegister(VEML7700_INTERRUPT_STATUS, isr.all);

    return rc != ksfTkErrOk ? VEML7700_INT_STATUS_INVALID : (VEML7700_interrupt_status_t)isr.INT_STATUS_REG_INT_FLAGS;
}

//----------------------------------------------------------------------------------------------------
// Convert the VEML7700_integration_time_t to a VEML7700_config_integration_time_t
//
sfDevVEML7700::VEML7700_config_integration_time_t sfDevVEML7700::integrationTimeConfig(VEML7700_integration_time_t it)
{
    // since the value of integration time is defined in a simple sequential enum order, and the intput is typed,
    // just use a translation table.
    VEML7700_config_integration_time_t table[] = {VEML7700_CONFIG_INTEGRATION_25ms,  VEML7700_CONFIG_INTEGRATION_50ms,
                                                  VEML7700_CONFIG_INTEGRATION_100ms, VEML7700_CONFIG_INTEGRATION_200ms,
                                                  VEML7700_CONFIG_INTEGRATION_400ms, VEML7700_CONFIG_INTEGRATION_800ms};

    return it < VEML7700_INTEGRATION_INVALID ? table[it] : VEML7700_CONFIG_INTEGRATION_INVALID;
}

//----------------------------------------------------------------------------------------------------
// Convert the VEML7700_config_integration_time_t to a VEML7700_integration_time_t
//
VEML7700_integration_time_t sfDevVEML7700::integrationTimeFromConfig(
    sfDevVEML7700::VEML7700_config_integration_time_t it)
{
    switch (it)
    {
    case VEML7700_CONFIG_INTEGRATION_25ms:
        return (VEML7700_INTEGRATION_25ms);
        break;
    case VEML7700_CONFIG_INTEGRATION_50ms:
        return (VEML7700_INTEGRATION_50ms);
        break;
    case VEML7700_CONFIG_INTEGRATION_100ms:
        return (VEML7700_INTEGRATION_100ms);
        break;
    case VEML7700_CONFIG_INTEGRATION_200ms:
        return (VEML7700_INTEGRATION_200ms);
        break;
    case VEML7700_CONFIG_INTEGRATION_400ms:
        return (VEML7700_INTEGRATION_400ms);
        break;
    case VEML7700_CONFIG_INTEGRATION_800ms:
        return (VEML7700_INTEGRATION_800ms);
        break;
    default:
        break;
    }
    return (VEML7700_INTEGRATION_INVALID);
}
