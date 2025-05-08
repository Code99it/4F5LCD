/**
 * mcp2515_init.h
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

#ifndef _MCP2515_INIT_H_
#define _MCP2515_INIT_H_

// Einbindung der SPI-Treiber-API des ESP-IDF
#include "driver/spi_master.h"

// Externer Handle für den SPI-Zugriff auf den Powertrain-MCP2515
extern spi_device_handle_t mcp_handle_powertrain;

// Initialisiert den SPI-Bus, registriert den MCP2515 und führt Hardware-Reset durch
esp_err_t init_spi_and_reset_mcp2515(void);

#endif // _MCP2515_INIT_H_
