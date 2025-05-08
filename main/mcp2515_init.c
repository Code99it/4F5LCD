/**
 * mcp2515_init.c
 *
 * Copyright (C) 05/2025 Christian Wilhelm, cw1@code99.it
 * 
 * Permission is granted to use this code for any private purpose;
 * commercial use or sale of this code is strictly prohibited.
 * 
 * Es ist gestattet, diesen Code privat beliebig zu verwenden oder zu ändern;
 * kommerzielle Nutzung oder Verkauf dieses Codes und damit verbundener 
 * Komplettlösungen oder Produkte ist strengstens untersagt.
 * 
 **/

// Projektweite Konfiguration und Pin-Zuweisungen
#include "defines.h"

// Header dieser Initialisierungsdatei
#include "mcp2515_init.h"

// ESP-IDF SPI-Master und GPIO-Funktionen
#include "driver/spi_master.h"
#include "driver/gpio.h"

// Logging für ESP32
#include "esp_log.h"

// Logging-Tag für diese Datei
static const char *TAG = "MCP2515_INIT";

// Handle für das SPI-Gerät des Powertrain-MCP2515
spi_device_handle_t mcp_handle_powertrain;

/**
 * Interruptroutine für Powertrain-MCP2515
 * Wird aktiv, wenn neue CAN-Nachricht empfangen wurde (RX0IF)
 * Aktuell noch leer – später Semaphore, Queue oder EventGroup möglich
 */
static void IRAM_ATTR powertrain_int_handler(void* arg)
{
    // Noch keine ISR-Logik – reserviert für zukünftige Verwendung
}

esp_err_t init_spi_and_reset_mcp2515(void)
{
    // Initialisierung des SPI-Busses für die MCP2515-Kommunikation
    ESP_LOGI(TAG, "Initializing SPI bus...");

    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    esp_err_t ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        // Fehler bei SPI-Bus Initialisierung
        ESP_LOGE(TAG, "SPI bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Reset-Pin als Output konfigurieren und kurzen Low-Puls erzeugen
    gpio_reset_pin(PIN_RST_POWERTRAIN);
    gpio_set_direction(PIN_RST_POWERTRAIN, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_RST_POWERTRAIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_RST_POWERTRAIN, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    // SPI-Gerät (MCP2515) mit Chip-Select-Pin registrieren
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,   // SPI-Takt: 10 MHz
        .mode = 0,
        .spics_io_num = PIN_CS_POWERTRAIN,
        .queue_size = 1,
    };

    ret = spi_bus_add_device(SPI_HOST, &devcfg, &mcp_handle_powertrain);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add MCP2515: %s", esp_err_to_name(ret));
        return ret;
    }

    // Soft-Reset des MCP2515 über SPI (0xC0)
    uint8_t reset_cmd = 0xC0;
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &reset_cmd,
    };

    ret = spi_device_transmit(mcp_handle_powertrain, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Soft reset failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Powertrain-CAN initialized.");

    // Konfiguration des INT-Pins (Eingang, mit Pull-Up)
    ESP_LOGI(TAG, "Configuring INT pin for MCP2515...");

    gpio_config_t int_conf = {
        .intr_type = GPIO_INTR_DISABLE,               // Noch kein aktiver Interrupt
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << PIN_INT_POWERTRAIN),
        .pull_up_en = GPIO_PULLUP_ENABLE,             // INT ist Open Drain → Pull-Up notwendig
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    ret = gpio_config(&int_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure INT pin: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "INT pin configured (no ISR yet).");

    // Erfolgreich abgeschlossen
    return ESP_OK;
}