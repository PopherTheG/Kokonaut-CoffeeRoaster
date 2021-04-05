#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "string.h"

#include "esp_log.h"

#define TAG "main.c"

#define PIN_NUM_MISO 3
#define PIN_NUM_CLK 5
#define PIN_NUM_CS 4
#define GPIO_OUTPUT_PIN_SEL (1ULL << PIN_NUM_CS)

// #define MAX31855_HOST SPI3_HOST

// void max31855_read32(spi_device_handle_t spi, uint32_t* data)
// {
//     spi_transaction_t t;
//     t.length = 32;
//     t.rxlength = 32;
//     t.tx_buffer = NULL;
//     t.rx_buffer = (void*)data;
//     // ESP_ERROR_CHECK(spi_device_acquire_bus(spi, portMAX_DELAY));
//     memset(&t, 0, sizeof(t));
//     spi_device_transmit(spi, &t);
//     // vTaskDelay(pdMS_TO_TICKS(10));
//     // spi_device_release_bus(spi);
// }

// void app_main(void)
// {
//     // esp_err_t ret;
//     spi_device_handle_t spi;
//     spi_bus_config_t buscfg = {
//         .miso_io_num = PIN_NUM_MISO,
//         .mosi_io_num = -1,
//         .sclk_io_num = PIN_NUM_CLK,
//         .quadhd_io_num = -1,
//         .quadwp_io_num = -1,
//         .max_transfer_sz = 0
//     };

//     spi_device_interface_config_t devcfg = {
//         .clock_speed_hz = 5 * 1000 * 1000,
//         .mode = 0,
//         .spics_io_num = PIN_NUM_CS,
//         .queue_size = 7
//     };

//     /* initialize a SPI bus */
//     spi_bus_initialize(MAX31855_HOST, &buscfg, 0);

//     /* register a device connected to the bus */
//     spi_bus_add_device(MAX31855_HOST, &devcfg, &spi);

//     while (1)
//     {
//         uint32_t raw_data = 0;
//         max31855_read32(spi, &raw_data);
//         ESP_LOGI(TAG, "0x%x", raw_data);
//         vTaskDelay(pdMS_TO_TICKS(500));
//     }
// }

void app_main()
{
    spi_host_device_t spi_host = SPI3_HOST;
    spi_device_handle_t spi;
    esp_err_t ret;

    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.mosi_io_num = -1;
    buscfg.miso_io_num = PIN_NUM_MISO;
    buscfg.sclk_io_num = PIN_NUM_CLK;
    buscfg.quadhd_io_num = -1;
    buscfg.quadwp_io_num = -1;
    buscfg.max_transfer_sz = 1;
    buscfg.flags = SPICOMMON_BUSFLAG_MASTER;
    buscfg.intr_flags = 0;

    spi_device_interface_config_t devcfg;
    memset(&devcfg, 0, sizeof(devcfg));
    devcfg.command_bits = 0;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.mode = (uint8_t)0;
    devcfg.duty_cycle_pos = 0;
    devcfg.cs_ena_pretrans = 0;
    devcfg.cs_ena_posttrans = (uint8_t)0;
    devcfg.clock_speed_hz = 5 * 1000 * 1000;
    devcfg.input_delay_ns = 50;
    devcfg.spics_io_num = PIN_NUM_CS;
    devcfg.flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_CLK_AS_CS;
    devcfg.queue_size = 1;
    devcfg.pre_cb = 0;
    devcfg.post_cb = 0;

    ret = spi_bus_initialize(spi_host, &buscfg, 0);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(spi_host, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    while (1)
    {
        spi_transaction_t t;
        spi_device_acquire_bus(spi, portMAX_DELAY);
        memset(&t, 0, sizeof(t));
        uint32_t rxBuffer;
        t.length = 32;
        t.rx_buffer = &rxBuffer;

        spi_device_polling_transmit(spi, &t);
        spi_device_release_bus(spi);

        ESP_LOGI("SPI", "0x%x", rxBuffer);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}