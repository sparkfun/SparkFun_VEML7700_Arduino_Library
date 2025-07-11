/*!
 * @file SparkFun_VEML7700_Arduino_Library.h
 *
 * SparkFun VEML7700 Ambient Light Sensor Arduino Library
 *
 * This library facilitates communication with the VEML7700 over I<sup>2</sup>C.
 *
 * Want to support open source hardware? Buy a board from SparkFun!
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
 */

#pragma once

#include <stdint.h>

// include the sparkfun toolkit headers
#include <sfTk/sfToolkit.h>

// Bus interfaces
#include <sfTk/sfTkII2C.h>

typedef uint16_t VEML7700_t;

/**  VEML7700 I2C address */
#define SF_VEML7700_DEFAULT_ADDRESS 0x10 // Default I2C address for the VEML7700 sensor

/** Sensitivity mode selection */
typedef enum
{
    VEML7700_SENSITIVITY_x1,
    VEML7700_SENSITIVITY_x2,
    VEML7700_SENSITIVITY_x1_8,
    VEML7700_SENSITIVITY_x1_4,
    VEML7700_SENSITIVITY_INVALID
} VEML7700_sensitivity_mode_t;

/** ALS integration time setting
    Note: these are defined here in simple sequential order.
          The actual register settings are defined in VEML7700_config_integration_time_t */
typedef enum
{
    VEML7700_INTEGRATION_25ms,
    VEML7700_INTEGRATION_50ms,
    VEML7700_INTEGRATION_100ms,
    VEML7700_INTEGRATION_200ms,
    VEML7700_INTEGRATION_400ms,
    VEML7700_INTEGRATION_800ms,
    VEML7700_INTEGRATION_INVALID
} VEML7700_integration_time_t;

/** ALS persistence protect number setting */
typedef enum
{
    VEML7700_PERSISTENCE_1,
    VEML7700_PERSISTENCE_2,
    VEML7700_PERSISTENCE_4,
    VEML7700_PERSISTENCE_8,
    VEML7700_PERSISTENCE_INVALID
} VEML7700_persistence_protect_t;

/** ALS interrupt enable setting */
typedef enum
{
    VEML7700_INT_DISABLE,
    VEML7700_INT_ENABLE,
    VEML7700_INT_INVALID
} VEML7700_interrupt_enable_t;
/** ALS interrupt status, logical OR of the crossing low and high threshold INT triggers */
typedef enum
{
    VEML7700_INT_STATUS_NONE,
    VEML7700_INT_STATUS_HIGH,
    VEML7700_INT_STATUS_LOW,
    VEML7700_INT_STATUS_BOTH,
    VEML7700_INT_STATUS_INVALID
} VEML7700_interrupt_status_t;

/** ALS shut down setting */
typedef enum
{
    VEML7700_POWER_ON,
    VEML7700_SHUT_DOWN,
    VEML7700_SHUTDOWN_INVALID
} VEML7700_shutdown_t;

const uint16_t kVEML7700ValueError = 0xFFFF; // Value returned when the VEML7700 is not connected or an error occurs

/** Communication interface for the VEML7700 */
class sfDevVEML7700
{
  public:
    sfDevVEML7700() : _theBus{nullptr}
    {
    }

    sfTkError_t begin(sfTkIBus *theBus = nullptr);

    bool isConnected();

    /** Configuration controls */

    sfTkError_t setShutdown(bool);
    sfTkError_t powerOn(void)
    {
        return setShutdown(false);
    };
    sfTkError_t shutdown(void)
    {
        return setShutdown(true);
    };
    bool isShutdown(void);

    sfTkError_t enableInterrupt(bool);
    bool interruptEnabled(void);

    sfTkError_t setPersistenceProtect(VEML7700_persistence_protect_t pp);
    VEML7700_persistence_protect_t persistenceProtect(void);
    const char *persistenceProtectString(void);

    sfTkError_t setIntegrationTime(VEML7700_integration_time_t it);
    VEML7700_integration_time_t integrationTime(void);
    const char *integrationTimeString(void);

    sfTkError_t setSensitivityMode(VEML7700_sensitivity_mode_t sm);
    VEML7700_sensitivity_mode_t sensitivityMode(void);
    const char *sensitivityModeString(void);

    sfTkError_t setHighThreshold(uint16_t threshold);
    uint16_t highThreshold(void);

    sfTkError_t setLowThreshold(uint16_t threshold);
    uint16_t lowThreshold(void);

    /** Read the sensor data */

    uint16_t readAmbientLight(void);
    uint16_t readWhiteLevel(void);
    float readLux(void);

    /** Note: reading the interrupt status register clears the interrupts.
              So, we need to check both interrupt flags in a single read. */
    VEML7700_interrupt_status_t interruptStatus(void);

  private:
    /** Provide bit field access to the configuration register */
    typedef struct
    {
        union {
            VEML7700_t all;
            struct
            {
                VEML7700_t CONFIG_REG_SD : 1;     // ALS shut down
                VEML7700_t CONFIG_REG_INT_EN : 1; // ALS interrupt enable
                VEML7700_t CONFIG_REG_RES1 : 2;   // Reserved
                VEML7700_t CONFIG_REG_PERS : 2;   // ALS persistence protect number
                VEML7700_t CONFIG_REG_IT : 4;     // ALS integration time
                VEML7700_t CONFIG_REG_RES2 : 1;   // Reserved
                VEML7700_t CONFIG_REG_SM : 2;     // ALS sensitivity mode
                VEML7700_t CONFIG_REG_RES3 : 3;   // Reserved
            };
        };
    } sfDevVEML7700Config_t;

    /** Provide bit field access to the interrupt status register
        Note: reading the interrupt status register clears the interrupts.
              So, we need to check both interrupt flags in a single read. */
    typedef struct
    {
        union {
            VEML7700_t all;
            struct
            {
                VEML7700_t INT_STATUS_REG_RES : 14; // Reserved
                // Bit 14 indicates if the high threshold was exceeded
                // Bit 15 indicates if the low threshold was exceeded
                VEML7700_t INT_STATUS_REG_INT_FLAGS : 2;
            };
        };
    } VEML7700_INTERRUPT_STATUS_REGISTER_t;

    /** VEML7700 Registers */
    typedef enum : uint8_t
    {
        VEML7700_CONFIGURATION_REGISTER,
        VEML7700_HIGH_THRESHOLD,
        VEML7700_LOW_THRESHOLD,
        VEML7700_ALS_OUTPUT = 4,
        VEML7700_WHITE_OUTPUT,
        VEML7700_INTERRUPT_STATUS
    } VEML7700_registers_t;

    /** ALS integration time setting */
    typedef enum
    {
        VEML7700_CONFIG_INTEGRATION_25ms = 0b1100,
        VEML7700_CONFIG_INTEGRATION_50ms = 0b1000,
        VEML7700_CONFIG_INTEGRATION_100ms = 0b0000,
        VEML7700_CONFIG_INTEGRATION_200ms = 0b0001,
        VEML7700_CONFIG_INTEGRATION_400ms = 0b0010,
        VEML7700_CONFIG_INTEGRATION_800ms = 0b0011,
        VEML7700_CONFIG_INTEGRATION_INVALID
    } VEML7700_config_integration_time_t;

    sfTkError_t updateConfiguration(sfDevVEML7700Config_t &config);

    /** Convert the (sequential) integration time into the corresponding (non-sequential) configuration value */
    VEML7700_config_integration_time_t integrationTimeConfig(VEML7700_integration_time_t it);
    /** Convert the (non-sequential) integration time config into the corresponding (sequential) integration time */
    VEML7700_integration_time_t integrationTimeFromConfig(VEML7700_config_integration_time_t it);

    sfTkIBus *_theBus;
};
