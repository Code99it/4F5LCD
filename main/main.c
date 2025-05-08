/**
 * main.c
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

// Standard-I/O
#include <stdio.h>

// FreeRTOS – grundlegende Task- und Timing-Funktionen
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP-IDF Logging-Modul
#include "esp_log.h"

// Initialisierung von SPI und MCP2515-Modulen
#include "mcp2515_init.h"

// Lesefunktion für CAN-Botschaften über MCP2515
#include "mcp2515_can_read.h"

// Platzhalter für zukünftige CAN-Schreibfunktionen
// #include "mcp2515_can_write.h"

// Lokaler Tag für ESP_LOG-Makros (zur Filterung im Log)
static const char *TAG = "MAIN";

void app_main(void)
{
    // Hinweis im Log, dass das System startet
    ESP_LOGI(TAG, "4F5LCD READY.");

    // Initialisiert SPI-Bus und führt Hardware-Reset sowie Konfiguration für MCP2515 durch
    if (init_spi_and_reset_mcp2515() != ESP_OK) {
        // Fehler beim Initialisieren – Logausgabe und Programmabbruch
        ESP_LOGE(TAG, "MCP2515 init failed!");
        return;
    }

    // Testweiser Lesezugriff auf eine CAN-Botschaft vom Powertrain-Bus
    read_can_frame_powertrain();

    // Endlosschleife – das System bleibt aktiv
    while (1) {
        // Kurze Pause, damit das Hauptprogramm nicht mit ununterbrochenem Dauerfeuer durchläuft
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}