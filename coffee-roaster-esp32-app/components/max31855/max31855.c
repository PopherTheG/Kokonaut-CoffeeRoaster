#include "max31855.h"



void max31855_init(uint8_t cs_pin, uint8_t clk_pin, uint8_t do_pin);
max31855_error_t max31855_readtemp(int16_t *temp_thermocouple, int16_t *temp_internal);