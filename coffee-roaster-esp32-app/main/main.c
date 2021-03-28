#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "esp_log.h"

#define TAG "main.c"

#define PIN_NUM_MISO 3
#define PIN_NUM_CLK 5
#define PIN_NUM_CS 4

#define MAX31855_HOST SPI2_HOST

static void configure_spi_bus(spi_host_device_t host_id, int clkPin, int mosiPin, int misoPin);
static uint32_t max31855_read_data();
static void max31855_add_to_spi_bus(int csPin);

spi_device_handle_t max31855_spi_handle;

void app_main(void)
{
    /* set up the GPIO pins to be used by the SPI2 bus */
    configure_spi_bus(SPI2_HOST, PIN_NUM_CLK, -1, PIN_NUM_MISO);

    /* add max31855 to the SPI bus */
    max31855_add_to_spi_bus(PIN_NUM_CS);

    while (1)
    {
        spi_device_acquire_bus(max31855_spi_handle, portMAX_DELAY);
        max31855_read_data();
        spi_device_release_bus(max31855_spi_handle);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* Bus definition identifies the pins used for a bus for MOSI, MISO, and CLK */
static void configure_spi_bus(spi_host_device_t host_id, int clkPin, int mosiPin, int misoPin)
{
    gpio_set_direction(misoPin, GPIO_MODE_INPUT);
    gpio_set_direction(clkPin, GPIO_MODE_OUTPUT);
    
    spi_bus_config_t bus_config;
    bus_config.sclk_io_num = clkPin;
    bus_config.mosi_io_num = mosiPin;
    bus_config.miso_io_num = misoPin;
    bus_config.quadhd_io_num = -1; // Not used
    bus_config.quadwp_io_num = -1; // Not used
    ESP_LOGI(TAG, "Initializing SPI");
    ESP_ERROR_CHECK(spi_bus_initialize(host_id, &bus_config, 0));
}

static void max31855_add_to_spi_bus(int csPin)
{
    gpio_set_direction(csPin, GPIO_MODE_OUTPUT);

    spi_device_interface_config_t dev_config;
    dev_config.address_bits = 0;
    dev_config.command_bits = 0;
    dev_config.dummy_bits = 0;
    dev_config.mode = 0;
    dev_config.duty_cycle_pos = 0;
    dev_config.cs_ena_posttrans = 0;
    dev_config.cs_ena_pretrans = 0;
    dev_config.clock_speed_hz = 5 * 1000000;
    dev_config.spics_io_num = csPin;
    dev_config.flags = 0;
    dev_config.queue_size = 1;
    dev_config.pre_cb = NULL;
    dev_config.post_cb = NULL;
    ESP_LOGI(TAG, "Adding device bus");
    ESP_ERROR_CHECK(spi_bus_add_device(MAX31855_HOST, &dev_config, &max31855_spi_handle));
}

/**
 * @brief   read data from the register of the max31855
 * @return 
 */
static uint32_t max31855_read_data()
{
    uint32_t data;
    spi_transaction_t trans_desc;
    trans_desc.addr = 0;
    trans_desc.cmd = 0;
    trans_desc.flags = 0;
    trans_desc.length = 4 * 8;
    trans_desc.rxlength = 4 * 8;
    trans_desc.tx_buffer = NULL;
    trans_desc.rx_buffer = &data;
    // data = 0xFFFFFFFF;
    ESP_LOGI(TAG, "...Transmitting");
    ESP_ERROR_CHECK(spi_device_transmit(max31855_spi_handle, &trans_desc));
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_LOGI(TAG, "data: %x", data);
    return data;
}

static void max31855_read_internal()
{
    uint32_t v;
}
