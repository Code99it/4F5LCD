/**
 * mcp2515_can_read.c
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

// Standardfunktionen
#include <string.h>

// Projektweite Definitionen (Pins, Indizes, etc.)
#include "defines.h"

// SPI-Initialisierung und MCP2515-Setup
#include "mcp2515_init.h"

// Header für CAN-Lesefunktion und Strukturdefinition
#include "mcp2515_can_read.h"

// SPI-Treiber des ESP-IDF
#include "driver/spi_master.h"

// Logging (ESP_LOG)
#include "esp_log.h"

// Logging-Tag für diese Datei
static const char *TAG = "CAN_READ";

// Interne Hilfsfunktion: Liest ein Register aus dem MCP2515
static esp_err_t mcp2515_read_register(spi_device_handle_t handle, uint8_t addr, uint8_t *out_value)
{
    // SPI-Befehl: 0x03 = READ, gefolgt von Adresse
    uint8_t cmd[2] = { 0x03, addr };
    uint8_t rx[1];

    spi_transaction_t t = {
        // 2 Bytes senden (READ + ADDR)
        .length = 16,          
        .tx_buffer = cmd,
        // 1 Byte lesen
        .rxlength = 8,         
        .rx_buffer = rx
    };

    // Übertrage SPI-Befehl
    esp_err_t ret = spi_device_transmit(handle, &t);
    if (ret == ESP_OK) {
        // Wert auslesen, wenn erfolgreich
        *out_value = rx[0];
    }
    return ret;
}

/**
 * Liest eine einzelne CAN-Nachricht aus dem RX-Buffer 0 des MCP2515 (Powertrain-CAN).
 * Voraussetzung: Der MCP2515 hat zuvor eine CAN-Nachricht empfangen und das RX0IF-Flag gesetzt.
 * Nach dem Lesen wird das RX0IF-Flag gelöscht, um den INT-Pin zurückzusetzen.
 */
esp_err_t read_can_frame_powertrain(can_frame_t *frame_out)
{
    uint8_t canintf = 0;

    // Schritt 1: CANINTF-Register lesen (Adresse 0x2C)
    // Dieses Register enthält unter anderem das RX0IF-Bit (Bit 0), das anzeigt,
    // ob eine neue Nachricht im RX-Buffer 0 empfangen wurde.
    esp_err_t ret = mcp2515_read_register(mcp_handle_powertrain, 0x2C, &canintf);
    if (ret != ESP_OK) {
        // Fehler beim SPI-Zugriff oder MCP nicht bereit
        return ret;
    }

    // Schritt 2: Prüfen, ob RX0IF-Bit gesetzt ist
    // Wenn nicht, gibt es aktuell keine neue Nachricht – Funktion endet mit ESP_ERR_NOT_FOUND
    if (!(canintf & 0x01)) {
        return ESP_ERR_NOT_FOUND;
    }

    // Schritt 3: RX-Buffer 0 auslesen (13 Byte)
    // Dazu wird der MCP2515-Befehl "READ RX BUFFER 0" (0x90) verwendet.
    // Es folgen automatisch SIDH, SIDL, EID8, EID0, DLC und bis zu 8 Datenbytes.
    uint8_t cmd = 0x90;
    uint8_t rx_data[13];

    spi_transaction_t t = {
        // 1 Byte senden (der Befehl 0x90)
        .length = 8,              
        .tx_buffer = &cmd,
        // 13 Bytes zurücklesen
        .rxlength = 13 * 8,       
        .rx_buffer = rx_data,
    };

    ret = spi_device_transmit(mcp_handle_powertrain, &t);
    if (ret != ESP_OK) {
        // Fehler beim SPI-Datentransfer
        return ret;
    }

    // Schritt 4: Empfangene Nachricht in die Zielstruktur frame_out eintragen
    // Die 11-Bit CAN-ID ist auf zwei Bytes verteilt (SIDH, SIDL)
    memset(frame_out, 0, sizeof(can_frame_t));
    frame_out->id = ((rx_data[0] << 3) | (rx_data[1] >> 5)) & 0x7FF;
    frame_out->dlc = rx_data[4] & 0x0F;
    memcpy(frame_out->data, &rx_data[5], frame_out->dlc);

    // Schritt 5: RX0IF-Flag manuell löschen, damit INT-Pin wieder freigegeben wird
    // Das passiert durch Schreiben von 0x01 in das CANINTF-Register (0x2C)
    // WRITE-Befehl: Adresse 0x2C, Daten 0x01
    uint8_t clear_cmd[3] = { 0x02, 0x2C, 0x01 }; 
    spi_transaction_t clear_t = {
        .length = 3 * 8,
        .tx_buffer = clear_cmd,
    };

    ret = spi_device_transmit(mcp_handle_powertrain, &clear_t);
    if (ret != ESP_OK) {
        // Wenn das Löschen fehlschlägt, INT bleibt LOW – Folgeprobleme möglich
        return ret;
    }

    // Alles erfolgreich
    return ESP_OK;
}
