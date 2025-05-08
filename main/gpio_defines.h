/**
 * defines.h
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

#ifndef _DEFINES_H_
#define _DEFINES_H_

// -----------------------------------------------------------------------------
// SPI-Konfiguration für die Kommunikation mit MCP2515 (CAN-Controller)
//
// Der ESP32 bietet mehrere SPI-Controller. Hier wird der HSPI (nicht VSPI) 
// verwendet, damit es zu keinen Konflikten mit internen Peripherien kommt.
// Die GPIO-Zuordnung entspricht einem gängigen Setup für den ESP32.
// -----------------------------------------------------------------------------

// Verwendeter SPI-Host (HSPI)
#define SPI_HOST         HSPI_HOST

// Master In Slave Out (Daten vom MCP2515 zum ESP32)
#define PIN_NUM_MISO     19

// Master Out Slave In (Daten vom ESP32 zum MCP2515)
#define PIN_NUM_MOSI     23

// SPI-Taktleitung
#define PIN_NUM_CLK      18

// -----------------------------------------------------------------------------
// MCP2515-Konfiguration für Powertrain-CAN
//
// Dieser Abschnitt legt fest, welcher Chip-Select (CS) und Reset-Pin verwendet 
// wird, um den MCP2515 zu kontrollieren, der den Powertrain-CAN überwacht.
// Diese Pins können bei Bedarf auf andere GPIOs gelegt werden, müssen aber
// mit der Schaltung übereinstimmen.
// -----------------------------------------------------------------------------

// GPIO für den CS-Pin des MCP2515 (aktiv LOW)
#define PIN_CS_POWERTRAIN   16

// GPIO für den Hardware-Reset des MCP2515 (aktiv LOW)
#define PIN_RST_POWERTRAIN  13

// -----------------------------------------------------------------------------
// Index-Konstante
//
// Wenn mehrere MCP2515-Instanzen verwendet werden (z. B. für Komfort- und 
// Infotainment-CAN), hilft dieser Index bei der Zuordnung in Arrays oder 
// Multiplex-Systemen. Momentan wird nur ein MCP verwendet.
// -----------------------------------------------------------------------------

// Index für den Powertrain-CAN-MCP im globalen Array
#define MCP_CAN_POWERTRAIN  0

// -----------------------------------------------------------------------------
// Interrupt-Pin für den MCP2515 (Powertrain)
//
// Der INT-Pin des MCP2515 signalisiert durch ein LOW-Signal, dass neue Daten 
// im Empfangspuffer liegen (z. B. CAN-Nachricht in RX0). Dieser Pin ist Open-Drain
// und benötigt daher einen externen Pull-Up-Widerstand (z. B. 10k).
// -----------------------------------------------------------------------------

// GPIO des ESP32, der mit INT des MCP2515 verbunden ist
#define PIN_INT_POWERTRAIN   4

#endif // _DEFINES_H_s