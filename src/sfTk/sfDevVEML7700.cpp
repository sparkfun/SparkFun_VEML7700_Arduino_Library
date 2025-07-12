/*!
 * @file SparkFun_VEML7700_Arduino_Library.h
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

/** The sensor resolution vs. gain and integration time. Taken from the VEML7700 Application Note. */
const float VEML7700_LUX_RESOLUTION[VEML7700_SENSITIVITY_INVALID][VEML7700_INTEGRATION_INVALID] = {
    // 25ms    50ms    100ms   200ms   400ms   800ms
    {0.2304, 0.1152, 0.0576, 0.0288, 0.0144, 0.0072}, // Gain (sensitivity) 1
    {0.1152, 0.0576, 0.0288, 0.0144, 0.0072, 0.0036}, // Gain (sensitivity) 2
    {1.8432, 0.9216, 0.4608, 0.2304, 0.1152, 0.0576}, // Gain (sensitivity) 1/8
    {0.9216, 0.4608, 0.2304, 0.1152, 0.0576, 0.0288}  // Gain (sensitivity) 1/4
};

/** The VEML7700 gain (sensitivity) settings as text (string) */
// Note: these are in the order defined by ALS_SM and VEML7700_sensitivity_mode_t
const char *kVEML7700GainSettingsString[VEML7700_SENSITIVITY_INVALID + 1] = {"x1", "x2", "x1/8", "x1/4", "INVALID"};

/** The VEML7700 integration time settings as text (string) */
const char *kVEML7799IntegrationTimesString[VEML7700_INTEGRATION_INVALID + 1] = {"25ms",  "50ms",  "100ms",  "200ms",
                                                                                 "400ms", "800ms", "INVALID"};

/** The VEML7700 persistence protect settings as text (string) */
const char *kVEML7700PersistenceProtectStrings[VEML7700_PERSISTENCE_INVALID + 1] = {"1", "2", "4", "8", "INVALID"};

//--------------------------------------------------------------------------------------------------
// Helpful method - used by a majority of methods
sfTkError_t sfDevVEML7700::updateConfiguration(sfDevVEML7700Config_t &config)
{
    if (_theBus == nullptr)
        return ksfTkErrBusNotInit; // Not initialized

    return _theBus->readRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}
//--------------------------------------------------------------------------------------------------
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
/**************************************************************************/
/*!
    @brief  Check that the VEML7700 is awake and communicating.
    @return True if communication with the VEML7700 was successful, otherwise false.
*/
/**************************************************************************/
bool sfDevVEML7700::isConnected(void)
{
    sfDevVEML7700Config_t config;
    return updateConfiguration(config) == ksfTkErrOk;
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's shut down setting (ALS_SD)
    @param  shutdown
            <br>The shut down setting. Possible values are:
            <br>VEML7700_POWER_ON
            <br>VEML7700_SHUT_DOWN

*/
/**************************************************************************/
sfTkError_t sfDevVEML7700::setShutdown(bool shutdown)
{

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the configuration register

    config.CONFIG_REG_SD = shutdown ? VEML7700_SHUT_DOWN : VEML7700_POWER_ON;

    return _theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's shut down setting (ALS_SD)
    @return VEML7700_POWER_ON or VEML7700_SHUT_DOWN if successful, VEML7700_SHUTDOWN_INVALID otherwise
*/
/**************************************************************************/
bool sfDevVEML7700::isShutdown(void)
{
    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return true; // Error reading the configuration register

    return (((VEML7700_shutdown_t)config.CONFIG_REG_SD) == VEML7700_SHUT_DOWN);
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's interrupt enable setting (ALS_INT_EN)
    @param  ie
            <br>The interrupt enable setting. Possible values are:
            <br>VEML7700_INT_DISABLE
            <br>VEML7700_INT_ENABLE
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
sfTkError_t sfDevVEML7700::enableInterrupt(bool bEnable)
{

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return rc; // Error reading the configuration register

    config.CONFIG_REG_INT_EN = (VEML7700_t)bEnable;

    return _theBus->writeRegister(VEML7700_CONFIGURATION_REGISTER, config.all);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's interrupt enable setting (ALS_INT_EN)
    @param  ie
            <br>Will be set to the interrupt enable setting on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
bool sfDevVEML7700::interruptEnabled(void)
{

    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return false; // Error reading the configuration register

    return config.CONFIG_REG_INT_EN == VEML7700_INT_ENABLE;
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's persistence protect number setting (ALS_PERS)
    @param  pp
            <br>The persistence protect setting. Possible values are:
            <br>VEML7700_PERSISTENCE_1
            <br>VEML7700_PERSISTENCE_2
            <br>VEML7700_PERSISTENCE_4
            <br>VEML7700_PERSISTENCE_8
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    @brief  Get the VEML7700's persistence protect number setting (ALS_PERS)
    @param  pp
            <br>Will be set to the persistence protect number on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_persistence_protect_t sfDevVEML7700::persistenceProtect(void)
{
    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return VEML7700_PERSISTENCE_INVALID; // Error reading the configuration register

    return (VEML7700_persistence_protect_t)config.CONFIG_REG_PERS;
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's persistence protect number setting (ALS_PERS) as printable text
*/
/**************************************************************************/
const char *sfDevVEML7700::persistenceProtectString(void)
{
    VEML7700_persistence_protect_t pp = persistenceProtect();

    if (pp < VEML7700_PERSISTENCE_1 || pp >= VEML7700_PERSISTENCE_INVALID)
        return "INVALID"; // Invalid persistence protect setting

    return kVEML7700PersistenceProtectStrings[pp];
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's integration time setting (ALS_IT)
            <br>Note: these are defined here in simple sequential order
            <br>The actual register settings are defined in VEML7700_config_integration_time_t
    @param  it
            <br>The integration time setting. Possible values are:
            <br>VEML7700_INTEGRATION_25ms
            <br>VEML7700_INTEGRATION_50ms
            <br>VEML7700_INTEGRATION_100ms
            <br>VEML7700_INTEGRATION_200ms
            <br>VEML7700_INTEGRATION_400ms
            <br>VEML7700_INTEGRATION_800ms
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    @brief  Get the VEML7700's integration time setting (ALS_IT)
    @return If successful:
            <br>VEML7700_INTEGRATION_25ms
            <br>VEML7700_INTEGRATION_50ms
            <br>VEML7700_INTEGRATION_100ms
            <br>VEML7700_INTEGRATION_200ms
            <br>VEML7700_INTEGRATION_400ms
            <br>VEML7700_INTEGRATION_800ms
            <br>Otherwise:
            <br>VEML7700_INTEGRATION_INVALID
*/
/**************************************************************************/
VEML7700_integration_time_t sfDevVEML7700::integrationTime(void)
{
    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return VEML7700_INTEGRATION_INVALID; // Error reading the configuration register

    return ((VEML7700_integration_time_t)integrationTimeFromConfig(
        (VEML7700_config_integration_time_t)config.CONFIG_REG_IT));
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's integration time setting (ALS_IT) as printable text
*/
/**************************************************************************/
const char *sfDevVEML7700::integrationTimeString(void)
{
    VEML7700_integration_time_t it = integrationTime();

    if (it < VEML7700_INTEGRATION_25ms || it >= VEML7700_INTEGRATION_INVALID)
        return "INVALID"; // Invalid integration time setting

    return kVEML7799IntegrationTimesString[it];
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's sensitivity mode selection (ALS_SM)
    @param  it
            <br>The sensitivity mode selection. Possible values are:
            <br>VEML7700_SENSITIVITY_x1
            <br>VEML7700_SENSITIVITY_x2
            <br>VEML7700_SENSITIVITY_x1_8
            <br>VEML7700_SENSITIVITY_x1_4
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    @brief  Get the VEML7700's sensitivity mode selection (ALS_SM)
    @return If successful:
            <br>VEML7700_SENSITIVITY_x1
            <br>VEML7700_SENSITIVITY_x2
            <br>VEML7700_SENSITIVITY_x1_8
            <br>VEML7700_SENSITIVITY_x1_4
            <br>Otherwise:
            <br>VEML7700_SENSITIVITY_INVALID
*/
/**************************************************************************/
VEML7700_sensitivity_mode_t sfDevVEML7700::sensitivityMode()
{
    sfDevVEML7700Config_t config;
    sfTkError_t rc = updateConfiguration(config);

    if (rc != ksfTkErrOk)
        return VEML7700_SENSITIVITY_INVALID; // Error reading the configuration register

    return ((VEML7700_sensitivity_mode_t)config.CONFIG_REG_SM);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's sensitivity mode selection (ALS_SM) as printable text
*/
/**************************************************************************/
const char *sfDevVEML7700::sensitivityModeString()
{
    VEML7700_sensitivity_mode_t sm = sensitivityMode();

    if (sm < VEML7700_SENSITIVITY_x1 || sm >= VEML7700_SENSITIVITY_INVALID)
        return "INVALID"; // Invalid sensitivity mode setting

    return kVEML7700GainSettingsString[sm];
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's ALS high threshold window setting (ALS_WH)
    @param  threshold
            <br>The threshold setting: 0x0000 to 0xFFFF
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
sfTkError_t sfDevVEML7700::setHighThreshold(uint16_t threshold)
{
    return _theBus->writeRegister(VEML7700_HIGH_THRESHOLD, threshold);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ALS high threshold window setting (ALS_WH)
    @return The threshold setting, or 0xFFFF on error
*/
/**************************************************************************/
uint16_t sfDevVEML7700::highThreshold(void)
{
    uint16_t threshold;
    sfTkError_t rc = _theBus->readRegister(VEML7700_HIGH_THRESHOLD, threshold);

    return rc == ksfTkErrOk ? threshold : kVEML7700ValueError; // Return 0xFFFF on error
}

/**************************************************************************/
/*!
    @brief  Set the VEML7700's ALS low threshold window setting (ALS_WL)
    @param  threshold
            <br>The threshold setting: 0x0000 to 0xFFFF
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
sfTkError_t sfDevVEML7700::setLowThreshold(uint16_t threshold)
{
    return _theBus->writeRegister(VEML7700_LOW_THRESHOLD, threshold);
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ALS low threshold window setting (ALS_WL)
    @return The threshold setting
*/
/**************************************************************************/
uint16_t sfDevVEML7700::lowThreshold(void)
{
    uint16_t threshold;
    sfTkError_t rc = _theBus->readRegister(VEML7700_LOW_THRESHOLD, threshold);

    return rc == ksfTkErrOk ? threshold : kVEML7700ValueError; // Return 0xFFFF on error
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's ambient light sensor data (ALS)
    @return The ambient light reading
*/
/**************************************************************************/
uint16_t sfDevVEML7700::readAmbientLight(void)
{

    uint16_t ambient;
    sfTkError_t rc = _theBus->readRegister(VEML7700_ALS_OUTPUT, ambient);

    return rc == ksfTkErrOk ? ambient : kVEML7700ValueError;
}

/**************************************************************************/
/*!
    @brief  Get the VEML7700's white level data (WHITE)
    @return The white level reading
*/
/**************************************************************************/
uint16_t sfDevVEML7700::readWhiteLevel(void)
{
    uint16_t whiteLevel;
    sfTkError_t rc = _theBus->readRegister(VEML7700_WHITE_OUTPUT, whiteLevel);

    return rc == ksfTkErrOk ? whiteLevel : kVEML7700ValueError;
}

/**************************************************************************/
/*!
    @brief  Read the sensor data and calculate the lux
    @param  lux
            <br>Will be set to the lux on return
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
float sfDevVEML7700::readLux(void)
{
    /** First, we need to extract the correct resolution from the VEML7700_LUX_RESOLUTION
        gain and integration time look up table. Let's begin by reading the gain
        (sensitivity) and integration time. */

    VEML7700_sensitivity_mode_t senseMode = sensitivityMode();

    if (senseMode == VEML7700_SENSITIVITY_INVALID)
        return 0.0;

    VEML7700_integration_time_t intTime = integrationTime();

    if (intTime == VEML7700_INTEGRATION_INVALID)
        return 0.0;

    /** Now we read the ambient level and multiply it by the resolution */
    uint16_t ambient = readAmbientLight();

    if (ambient == kVEML7700ValueError)
        return 0.0;

    // Apply the resolution to the ambient light reading
    return (float)ambient * VEML7700_LUX_RESOLUTION[senseMode][intTime];
}

/**************************************************************************/
/*!
    @brief  Read the VEML7700's interrupt status register
            <br>Note: reading the interrupt status register clears the interrupts.
            <br>      So, we need to check both interrupt flags in a single read.
    @param  status
            <br>Will be set to the logical OR of ALS_IF_L and ALS_IF_H on return
            <br>Possible values are:
            <br>VEML7700_INT_STATUS_NONE
            <br>VEML7700_INT_STATUS_HIGH
            <br>VEML7700_INT_STATUS_LOW
            <br>VEML7700_INT_STATUS_BOTH
            <br>If an I2C error occurred, status will be:
            <br>VEML7700_INT_STATUS_INVALID
    @return VEML7700_SUCCESS (VEML7700_ERROR_SUCCESS) if successful
*/
/**************************************************************************/
VEML7700_interrupt_status_t sfDevVEML7700::interruptStatus(void)
{

    VEML7700_INTERRUPT_STATUS_REGISTER_t isr;

    sfTkError_t rc = _theBus->readRegister(VEML7700_INTERRUPT_STATUS, isr.all);

    return rc != ksfTkErrOk ? VEML7700_INT_STATUS_INVALID : (VEML7700_interrupt_status_t)isr.INT_STATUS_REG_INT_FLAGS;
}

sfDevVEML7700::VEML7700_config_integration_time_t sfDevVEML7700::integrationTimeConfig(VEML7700_integration_time_t it)
{
    // since the value of integration time is defined in a simple sequential order, and the intput is typed,
    // just use a translation table.
    VEML7700_config_integration_time_t table[] = {VEML7700_CONFIG_INTEGRATION_25ms,  VEML7700_CONFIG_INTEGRATION_50ms,
                                                  VEML7700_CONFIG_INTEGRATION_100ms, VEML7700_CONFIG_INTEGRATION_200ms,
                                                  VEML7700_CONFIG_INTEGRATION_400ms, VEML7700_CONFIG_INTEGRATION_800ms};

    return it < VEML7700_INTEGRATION_INVALID ? table[it] : VEML7700_CONFIG_INTEGRATION_INVALID;
}

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
