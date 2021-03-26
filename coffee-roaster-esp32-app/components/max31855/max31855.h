#ifndef COMPONENTS_MAX31855_H
#define COMPONENTS_MAX31855_H

#include <stdint.h>

typedef enum
{
    MAX31855_OK = 0,
    MAX31855_NO_THERMOCOUPLE = -1,
    MAX31855_THERMOCOUPLE_SHORT_GND = -2,
    MAX31855_THERMOCOUPLE_SHORT_VCC = -3,
} max31855_error_t;

/**
 * @brief               Initialize the sensor.
 * 
 * @param   cs_pin      GPIO to use for chip select.
 * @param   clk_pin     GPIO to use for clock.
 * @param   do_pin      GPIO to use for data out.
 */
void max31855_init(uint8_t cs_pin, uint8_t clk_pin, uint8_t do_pin);

/**
 * @brief                           Read data from MAX31855.
 * 
 * @param   temp_thermocouple       Pointer to int16_t for thermocouple temperature. Value x10 (e.g. 982 = 98.2).
 * @param   temp_internal           Pointer to int16_t for internal reference temperature. Value x100 (e.g. 2337 = 23.37).
 * 
 * @return                          Returns MAX_OK (0) on success or error (negative value, see max31855_error_t).   
 */ 
max31855_error_t max31855_readtemp(int16_t *temp_thermocouple, int16_t *temp_internal);

#endif