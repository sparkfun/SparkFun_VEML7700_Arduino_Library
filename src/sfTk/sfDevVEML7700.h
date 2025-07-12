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
/**
 * @class sfDevVEML7700
 * @brief Driver class for the VEML7700 ambient light sensor.
 *
 * This class provides an interface to configure and read data from the VEML7700 sensor.
 * It supports sensor initialization, configuration of operational parameters, threshold settings,
 * interrupt management, and reading ambient light and white level measurements.
 *
 * Usage:
 * - Call begin() to initialize the sensor with an optional I2C bus.
 * - Use configuration methods to set shutdown, interrupt, persistence, integration time, sensitivity, and thresholds.
 * - Use data reading methods to obtain ambient light, white level, and lux values.
 * - Interrupt status can be checked and cleared via interruptStatus().
 *
 */
class sfDevVEML7700
{
  public:
    sfDevVEML7700() : _theBus{nullptr}
    {
    }

    /**
     * @brief Initializes the VEML7700 device on the specified bus.
     *
     * This function sets up communication with the VEML7700 sensor using the provided bus interface.
     * If no bus is specified, the method will fail and return an error.
     *
     * @param theBus Pointer to the bus interface (sfTkIBus) to use for communication. Defaults to nullptr.
     * @return sfTkError_t Error code indicating the result of the initialization.
     */
    sfTkError_t begin(sfTkIBus *theBus = nullptr);

    /**
     * @brief Checks if the VEML7700 sensor is connected and responding.
     *
     * @return true if the sensor is detected and communication is successful, false otherwise.
     */
    bool isConnected();

    /** Configuration controls */

    /**
     * @brief Sets the shutdown mode of the VEML7700 device.
     *
     * This function enables or disables the shutdown mode of the sensor.
     * When shutdown mode is enabled, the device enters a low-power state.
     *
     * @param shutdown If true, the device will enter shutdown mode; if false, it will operate normally.
     * @return sfTkError_t Error code indicating the result of the operation.
     */
    sfTkError_t setShutdown(bool);
    /**
     * @brief Powers on the VEML7700 device by clearing the shutdown mode.
     *
     * This function calls setShutdown(false) to enable the device's operation.
     *
     * @return sfTkError_t Error code indicating the result of the operation.
     */
    sfTkError_t powerOn(void)
    {
        return setShutdown(false);
    };
    /**
     * @brief Puts the VEML7700 device into shutdown mode.
     *
     * This function calls setShutdown(true) to transition the device into a low-power state.
     *
     * @return sfTkError_t Error code indicating the result of the shutdown operation.
     */
    sfTkError_t shutdown(void)
    {
        return setShutdown(true);
    };
    /**
     * @brief Checks if the VEML7700 device is currently in shutdown mode.
     *
     * This function returns true if the device is in shutdown mode, meaning it is not actively measuring.
     * Otherwise, it returns false.
     *
     * @return true if the device is shut down, false otherwise.
     */
    bool isShutdown(void);

    /**
     * @brief Enables or disables the interrupt functionality of the VEML7700 device.
     *
     * When enabled, the device will generate an interrupt signal based on its configuration.
     *
     * @param enable Set to true to enable interrupts, false to disable.
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t enableInterrupt(bool);

    /**
     * @brief Checks if the interrupt feature is enabled on the VEML7700 device.
     *
     * @return true if the interrupt is enabled, false otherwise.
     */
    bool isInterruptEnabled(void);

    /**
     * @brief Sets the persistence protection mode for the VEML7700 sensor.
     *
     * This function configures the persistence protection setting, which determines
     * how many consecutive out-of-threshold events are required before an interrupt is triggered.
     *
     * @param pp The persistence protection mode to set. This should be a value of type VEML7700_persistence_protect_t.
     *   Valid values are:
     *      VEML7700_PERSISTENCE_1
     *      VEML7700_PERSISTENCE_2
     *      VEML7700_PERSISTENCE_4
     *      VEML7700_PERSISTENCE_8
     * @return sfTkError_t Returns an error code indicating the success or failure of the operation.
     */
    sfTkError_t setPersistenceProtect(VEML7700_persistence_protect_t pp);

    /**
     * @brief Retrieves the current persistence protection setting from the VEML7700 sensor.
     *
     * This function queries the sensor for its persistence protection configuration,
     * which determines how many consecutive out-of-threshold measurements are required
     * before an interrupt is triggered.
     *
     * @param[out] pp Reference to a VEML7700_persistence_protect_t variable where the persistence protection value will
     * be stored.
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t getPersistenceProtect(VEML7700_persistence_protect_t &pp);

    /**
     * @brief Gets the current persistence protection setting of the VEML7700 sensor.
     *
     * This function returns the persistence protection configuration, which determines
     * how many consecutive out-of-range measurements are required before an interrupt is triggered.
     *
     * @return VEML7700_persistence_protect_t The current persistence protection setting.
     */
    VEML7700_persistence_protect_t persistenceProtect(void);
    /**
     * @brief Returns a string representation of the current persistence protection setting.
     *
     * This function provides a human-readable string that describes the current persistence protection mode.
     *
     * @return const char* A string representing the persistence protection setting.
     */
    const char *persistenceProtectString(void);

    /**
     * @brief Get the Persistence Protect object - this is for backward compatibility.
     *
     * @deprecated since version 2.0.0, use persistenceProtect() instead.
     * @return VEML7700_persistence_protect_t
     */
    const char *getPersistenceProtectStr(void)
    {
        return persistenceProtectString();
    }

    /**
     * @brief Sets the integration time for the VEML7700 sensor.
     *
     * This function configures the integration time, which determines the duration
     * over which the sensor accumulates light before taking a measurement. Adjusting
     * the integration time can affect the sensitivity and range of the sensor readings.
     *
     * @param it The desired integration time, specified as a value of the
     *           VEML7700_integration_time_t enumeration.
     * @return sfTkError_t Returns an error code indicating success or failure of the operation.
     */
    sfTkError_t setIntegrationTime(VEML7700_integration_time_t it);

    /**
     * @brief Retrieves the current integration time setting from the VEML7700 sensor.
     *
     * This function reads the sensor's configuration to determine the currently set integration time.
     *
     * @param[out] it Reference to a VEML7700_integration_time_t variable where the integration time will be stored.
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t getIntegrationTime(VEML7700_integration_time_t &it);

    /**
     * @brief Gets the current integration time setting of the VEML7700 sensor.
     *
     * This function retrieves the integration time that is currently configured for the sensor.
     *
     * @return VEML7700_integration_time_t The current integration time setting.
     */
    VEML7700_integration_time_t integrationTime(void);

    /**
     * @brief Get the Integration Time object -  this is for backward compatibility.
     *
     * @deprecated since version 2.0.0, use integrationTime() instead.
     * @return VEML7700_integration_time_t
     */

    VEML7700_integration_time_t getIntegrationTime(void)
    {
        return integrationTime();
    }
    /**
     * @brief Returns a string representation of the current integration time setting.
     *
     * This function provides a human-readable string that describes the current integration time.
     *
     * @return const char* A string representing the integration time setting.
     */
    const char *integrationTimeString(void);

    /**
     * @brief Get the Integration Time string - this is for backward compatibility.
     *
     * @deprecated since version 2.0.0, use integrationTimeString() instead.
     * @return const char* A string representing the integration time setting.
     */
    const char *getIntegrationTimeStr(void)
    {
        return integrationTimeString();
    }

    /**
     * @brief Sets the sensitivity mode for the VEML7700 sensor.
     *
     * This function configures the sensor's sensitivity mode, which affects
     * the measurement range and resolution. The sensitivity mode should be
     * selected based on the application's lighting conditions.
     *
     * @param sm The desired sensitivity mode, specified as a VEML7700_sensitivity_mode_t enum value.
     * @return sfTkError_t Returns an error code indicating success or failure of the operation.
     */
    sfTkError_t setSensitivityMode(VEML7700_sensitivity_mode_t sm);

    /**
     * @brief Retrieves the current sensitivity mode setting from the VEML7700 sensor.
     *
     * This function reads the sensor's configuration to determine the currently set sensitivity mode.
     *
     * @param[out] sm Reference to a VEML7700_sensitivity_mode_t variable where the sensitivity mode will be stored.
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t getSensitivityMode(VEML7700_sensitivity_mode_t &sm);

    /**
     * @brief Gets the current sensitivity mode setting of the VEML7700 sensor.
     *
     * This function retrieves the sensitivity mode that is currently configured for the sensor.
     *
     * @return VEML7700_sensitivity_mode_t The current sensitivity mode setting.
     */
    VEML7700_sensitivity_mode_t sensitivityMode(void);

    /**
     * @brief Get the Sensitivity Mode object - this is for backward compatibility.
     *
     * @deprecated since version 2.0.0, use sensitivityMode() instead.
     * @return VEML7700_sensitivity_mode_t
     */
    VEML7700_sensitivity_mode_t getSensitivityMode(void)
    {
        return sensitivityMode();
    }

    /**
     * @brief Returns a string representation of the current sensitivity mode setting.
     *
     * This function provides a human-readable string that describes the current sensitivity mode.
     *
     * @return const char* A string representing the sensitivity mode setting.
     */
    const char *sensitivityModeString(void);

    /**
     * @brief Get the Sensitivity Mode string - this is for backward compatibility.
     *
     * @deprecated since version 2.0.0, use sensitivityModeString() instead.
     * @return const char* A string representing the sensitivity mode setting.
     */
    const char *getSensitivityModeStr(void)
    {
        return sensitivityModeString();
    }

    /**
     * @brief Sets the high threshold value for the VEML7700 sensor.
     *
     * This function configures the high threshold for the sensor's interrupt.
     * When the measured value exceeds this threshold, an interrupt will be triggered.
     *
     * @param threshold The high threshold value to set (16-bit unsigned integer): 0x0000 to 0xFFFF
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t setHighThreshold(uint16_t threshold);

    /**
     * @brief Retrieves the current high threshold value from the VEML7700 sensor.
     *
     * This function reads the sensor's configuration to obtain the currently set high threshold value.
     *
     * @param[out] threshold Reference to a uint16_t variable where the high threshold will be stored.
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t getHighThreshold(uint16_t &threshold);

    /**
     * @brief Gets the current high threshold value of the VEML7700 sensor.
     *
     * This function retrieves the high threshold that is currently configured for the sensor.
     *
     * @return uint16_t The current high threshold value (16-bit unsigned integer): 0x0000 to 0xFFFF
     */
    uint16_t highThreshold(void);

    /**
     * @brief Get the High Threshold object - this is for backward compatibility.
     *
     * @deprecated since version 2.0.0, use highThreshold() instead.
     * @return uint16_t The current high threshold value (16-bit unsigned integer): 0x0000 to 0xFFFF
     */
    uint16_t getHighThreshold(void)
    {
        return highThreshold();
    }

    /**
     * @brief Sets the low threshold value for the VEML7700 sensor.
     *
     * This function configures the sensor to trigger an interrupt or event when the measured value
     * falls below the specified threshold.
     *
     * @param threshold The low threshold value to set (16-bit unsigned integer): 0x0000 to 0xFFFF
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t setLowThreshold(uint16_t threshold);

    /**
     * @brief Retrieves the current low threshold value from the VEML7700 sensor.
     *
     * This function reads the sensor's configuration to obtain the currently set low threshold value.
     *
     * @param[out] threshold Reference to a uint16_t variable where the low threshold will be stored.
     * @return sfTkError_t Error code indicating success or failure of the operation.
     */
    sfTkError_t getLowThreshold(uint16_t &threshold);

    /**
     * @brief Gets the current low threshold value of the VEML7700 sensor.
     *
     * This function retrieves the low threshold that is currently configured for the sensor.
     *
     * @return uint16_t The current low threshold value (16-bit unsigned integer): 0x0000 to 0xFFFF
     */
    uint16_t lowThreshold(void);

    /**
     * @brief Get the Low Threshold object - this is for backward compatibility.
     *
     * @deprecated since version 2.0.0, use lowThreshold() instead.
     * @return uint16_t The current low threshold value (16-bit unsigned integer): 0x0000 to 0xFFFF
     */
    uint16_t getLowThreshold(void)
    {
        return lowThreshold();
    } // Get the Low Threshold object - this is for backward compatibility

    /** Read the sensor data */

    /**
     * @brief Retrieves the ambient light measurement from the VEML7700 sensor.
     *
     * This function reads the current ambient light value and stores it in the provided reference variable.
     *
     * @param[out] ambient Reference to a uint16_t variable where the ambient light value will be stored.
     * @return sfTkError_t Error code indicating the status of the operation.
     */
    sfTkError_t getAmbientLight(uint16_t &ambient);
    /**
     * @brief Reads the ambient light measurement from the VEML7700 sensor.
     *
     * This function retrieves the current ambient light value.
     *
     * @return uint16_t The ambient light value (16-bit unsigned integer): 0x0000 to 0xFFFF
     */
    uint16_t getAmbientLight(void);

    /**
     * @brief Retrieves the current white light level measured by the VEML7700 sensor.
     *
     * This function reads the white channel value from the sensor and stores it in the provided reference variable.
     *
     * @param[out] whiteLevel Reference to a uint16_t variable where the white light level will be stored.
     * @return sfTkError_t Error code indicating the status of the operation.
     */
    sfTkError_t getWhiteLevel(uint16_t &whiteLevel);
    /**
     * @brief Reads the current white light level from the VEML7700 sensor.
     *
     * This function retrieves the white channel value measured by the sensor.
     *
     * @return uint16_t The white light level (16-bit unsigned integer): 0x0000 to 0xFFFF
     */
    uint16_t getWhiteLevel(void);

    /**
     * @brief Retrieves the current ambient light level in lux.
     *
     * This function reads the sensor and calculates the ambient light intensity,
     * storing the result in the provided reference variable.
     *
     * @param lux Reference to a float variable where the measured lux value will be stored.
     * @return sfTkError_t Error code indicating success or type of failure.
     */
    sfTkError_t getLux(float &lux);

    /**
     * @brief Reads the current ambient light level in lux.
     *
     * This function calculates the lux value based on the sensor's ambient light measurement.
     *
     * @return float The calculated lux value, or kVEML7700ValueError if an error occurs.
     */
    float getLux(void);

    /**
     * @brief Retrieves the current interrupt status from the VEML7700 sensor.
     *
     * This function queries the VEML7700 sensor for its interrupt status,
     * indicating whether a threshold event has occurred (such as crossing
     * high or low limits). The returned status can be used to determine if
     * further action is required, such as reading sensor data or clearing
     * the interrupt.
     *
     * @return VEML7700_interrupt_status_t The current interrupt status of the sensor.
     *
     * @note Reading the interrupt status register clears the interrupts.
           So, it is important to check both interrupt flags in a single read.
     */
    VEML7700_interrupt_status_t interruptStatus(void);

    /**
     * @brief Gets the interrupt status of the VEML7700 sensor.
     *
     * @deprecated since version 2.0.0, use interruptStatus() instead.
     * @return VEML7700_interrupt_status_t The current interrupt status of the sensor.
     */
    VEML7700_interrupt_status_t getInterruptStatus(void)
    {
        return interruptStatus();
    } // Get the Interrupt Status object - this is for backward compatibility

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

    // VEML7700 Registers - note types are uint8_t to match the I2C register size

    static constexpr uint8_t VEML7700_CONFIGURATION_REGISTER = 0;
    static constexpr uint8_t VEML7700_HIGH_THRESHOLD = 1;
    static constexpr uint8_t VEML7700_LOW_THRESHOLD = 2;
    static constexpr uint8_t VEML7700_ALS_OUTPUT = 4;
    static constexpr uint8_t VEML7700_WHITE_OUTPUT = 5;
    static constexpr uint8_t VEML7700_INTERRUPT_STATUS = 6;

    // ALS integration time setting
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

    /**
     * @brief Updates the device configuration with the provided settings.
     *
     * This function applies the configuration specified in the given
     * sfDevVEML7700Config_t structure to the VEML7700 device. It updates
     * the device registers according to the new configuration values.
     *
     * @param config Reference to a sfDevVEML7700Config_t structure containing
     *               the desired configuration settings.
     * @return sfTkError_t Error code indicating the result of the operation.
     *         Returns sfTkError_t::SFTK_OK on success, or an appropriate error code
     *         if the update fails.
     */
    sfTkError_t updateConfiguration(sfDevVEML7700Config_t &config);

    /**
     * @breif Convert the (sequential) integration time into the corresponding (non-sequential) configuration value
     *
     */
    VEML7700_config_integration_time_t integrationTimeConfig(VEML7700_integration_time_t it);

    /**
     * @breif Convert the (non-sequential) integration time config into the corresponding (sequential) integration time
     *
     */
    VEML7700_integration_time_t integrationTimeFromConfig(VEML7700_config_integration_time_t it);

    /**
     * Pointer to the I2C bus interface used for communication with the VEML7700 sensor.
     */
    sfTkIBus *_theBus;
};
