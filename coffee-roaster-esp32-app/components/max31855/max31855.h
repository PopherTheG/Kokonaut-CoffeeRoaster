#ifndef COMPONENTS_MAX31855_H
#define COMPONENTS_MAX31855_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#define PIN_NUM_MISO 3
#define PIN_NUM_CLK 5
#define PIN_NUM_CS 4

typedef struct max31855_cfg_t {
    spi_device_handle_t  spi;
    double thermocouple_temp;
    uint8_t fault_bit;
    double internal_temp;
    uint8_t scv_bit;
    uint8_t scg_bit;
    uint8_t oc_bit;
} max31855_cfg_t;

max31855_cfg_t max31855_init();

#endif

