#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"

#include "esp_log.h"

#define TAG "main.c"

#define PIN_NUM_MISO 3
#define PIN_NUM_CLK 5
#define PIN_NUM_CS 4

#define MAX31855_HOST SPI2_HOST

static void configure_spi_bus(int clkPin, int mosiPin, int misoPin);
static void max31855_spi_send_data();
static void max31855_spi_config(int csPin);
void test_spi_task(void *pvParamters);

typedef struct
{
  int32_t thermocouple_temp_data : 14;
  uint32_t res2 : 1;
  uint32_t fault_bit : 1;
  int32_t internal_temp_data : 12;
  uint32_t res1 : 1;
  uint32_t short_to_vcc : 1;
  uint32_t short_to_gnd : 1;
  uint32_t open_circuit : 1;
} max31855_datapacket_t;

max31855_datapacket_t recvData = {
    .thermocouple_temp_data = -377,
    .fault_bit = 1,
    .internal_temp_data = 28,
    .short_to_vcc = 0,
    .short_to_gnd = 0,
    .open_circuit = 0};

void app_main(void)
{

  ESP_LOGI(TAG, "%d", recvData.thermocouple_temp_data);
  ESP_LOGI(TAG, "%d", recvData.fault_bit);
  ESP_LOGI(TAG, "%d", recvData.internal_temp_data);
  ESP_LOGI(TAG, "%d", recvData.short_to_vcc);
  ESP_LOGI(TAG, "%d", recvData.short_to_gnd);
  ESP_LOGI(TAG, "%d", recvData.open_circuit);

  ESP_LOGI(TAG, "%u", sizeof(max31855_datapacket_t));

  configure_spi_bus(PIN_NUM_CLK, -1, PIN_NUM_MISO);

  xTaskCreate(test_spi_task, "test", 2048, NULL, 2, NULL);
}
// 09484207414

/* Bus definition identifies the pins used for a bus for MOSI, MISO, and CLK */
static void configure_spi_bus(int clkPin, int mosiPin, int misoPin)
{
  spi_bus_config_t bus_config;
  bus_config.sclk_io_num = clkPin;
  bus_config.mosi_io_num = mosiPin;
  bus_config.miso_io_num = misoPin;
  bus_config.quadhd_io_num = -1; // Not used
  bus_config.quadwp_io_num = -1; // Not used
  ESP_LOGI(TAG, "Initializing SPI");
  ESP_ERROR_CHECK(spi_bus_initialize(MAX31855_HOST, &bus_config, 1));
}
spi_device_handle_t handle;
static void max31855_spi_config(int csPin)
{

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
  ESP_ERROR_CHECK(spi_bus_add_device(MAX31855_HOST, &dev_config, &handle));
}

static void max31855_spi_send_data()
{
  char data[4];
  spi_transaction_t trans_desc;
  trans_desc.addr = 0;
  trans_desc.cmd = 0;
  trans_desc.flags = 0;
  trans_desc.length = 4 * 8;
  trans_desc.rxlength = 4 * 8;
  trans_desc.tx_buffer = data;
  trans_desc.rx_buffer = data;
  data[0] = 0xFF;
  data[1] = 0xFF;
  data[2] = 0xFF;
  data[3] = 0xFF;
  ESP_LOGI(TAG, "...Transmitting");
  ESP_ERROR_CHECK(spi_device_transmit(handle, &trans_desc));
  vTaskDelay(pdMS_TO_TICKS(1000));
  ESP_LOGI(TAG, "data: %x %x %x %x", data[3], data[2], data[1], data[0]);
  ESP_LOGI(TAG, "...Removing device");
  ESP_ERROR_CHECK(spi_bus_remove_device(handle));
  ESP_LOGI(TAG, "...Freeing bus");
  ESP_ERROR_CHECK(spi_bus_free(MAX31855_HOST));
}

void test_spi_task(void *pvParamters)
{
  max31855_spi_config(PIN_NUM_CS);
  while (1)
  {
    max31855_spi_send_data();
  }
  vTaskDelete(NULL);
}
