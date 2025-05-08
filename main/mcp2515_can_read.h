/**
 * mcp2515_can_read.h
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

#ifndef _MCP2515_CAN_READ_H_
#define _MCP2515_CAN_READ_H_

// ESP-IDF Fehlercodes (esp_err_t)
#include "esp_err.h"

// Struktur zur Repräsentation eines CAN-Datenrahmens (Standard-ID 11 Bit)
typedef struct 
{
    // 11-Bit CAN-Identifier (Standard-Format, keine Extended-ID)
    uint16_t id;

    // Data Length Code (Anzahl der gültigen Datenbytes im Datenfeld)
    uint8_t  dlc;

    // Datenfeld mit bis zu 8 Byte Nutzlast
    uint8_t  data[8];

} can_frame_t;

// Liest eine einzelne CAN-Botschaft vom Powertrain-CAN (MCP2515).
// Gibt ESP_OK zurück, wenn eine Nachricht empfangen wurde, 
// oder ESP_ERR_NOT_FOUND, wenn keine neue Nachricht vorliegt.
esp_err_t read_can_frame_powertrain(can_frame_t *frame_out);

#endif // _MCP2515_CAN_READ_H_
