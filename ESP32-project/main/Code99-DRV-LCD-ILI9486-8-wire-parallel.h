#include <stdbool.h>

#ifndef CODE99_DRV_LCD_ILI9486_8_WIRE_PARALLEL_H
#define CODE99_DRV_LCD_ILI9486_8_WIRE_PARALLEL_H

// Set Data Pins
void lcd_ili9486_data (uint8_t data); 

// GPIO Pin control for WRX (WR = Write Strobe);
//
void lcd_ili9486_pin_wrx (bool state); 

// GPIO Pin control for DCX (RS = Register Select);
//
void lcd_ili9486_pin_dcx (bool state); 

// GPIO Pin control for RDX (RD = Read);
//
void lcd_ili9486_pin_rdx (bool state); 

// GPIO Pin control for CS (Chip select);
//
void lcd_ili9486_pin_cs (bool state); 

// GPIO Pin control for RST (Reset)
//
void lcd_ili9486_pin_rst (bool state);

// A short pulse on the WRX pin (WR = Write Strobe);
//
void lcd_ili9486_write_strobe (); 

// Hard reset
//
void lcd_ili9486_hard_reset ();

// Soft Reset 01h
//
// D/CX 0
// RDX 1
// WRX rising
//
// D15:8 none
// D7:D0 00000001
//
// When the Software Reset command is written, it causes software reset. It resets the commands and parameters to their
// S/W Reset default values. (See default tables in each command description.);
// The display is blank immediately
// Note: The Frame Memory contents is kept or not by this command.
// It will be necessary to wait 5msec before sending new command following software reset. The display module loads all
// display supplier factory default values to the registers during this 5msec. If Software Reset is applied during Sleep Out
// mode, it will be necessary to wait 120msec before sending Sleep out command. Software Reset Command cannot be sent
// during Sleep Out sequence.
//
void lcd_ili9486_soft_reset (); 

// 8.2.15. Normal Display Mode ON (13h);
//
// This command returns the display to normal mode. Normal display mode on means Partial mode off and Scroll mode off.
//
void lcd_ili9486_normal_display_mode_on (); 


// 8.2.19. Display ON (29h);
//
// This command causes ILI9486 to start displaying the image data on the display device. The frame memory contents remain
// unchanged. No status bits are changed.
//
void lcd_ili9486_display_on (); 

// 8.2.20. Column Address Set (2Ah);
//
// This command is used to define area of frame memory where MCU can access. This command makes no change on the
// other driver status. The values of SC[15:0] and EC[15:0] are referred when RAMWR command comes. Each value
// represents one column line in the Frame Memory.
//
// 1st Parameter SC15:8  start column
// 2nd Parameter SC7:0   start column
// 3rd Parameter EC15:8  end column
// 4th Parameter EC7:0   end column
// 
void lcd_ili9486_column_address_set (uint8_t sc_15_to_8, uint8_t sc_7_to_0, uint8_t ec_15_to_8, uint8_t ec_7_to_0); 

// 8.2.21. Page Address Set (2Bh);
//
// This command is used to define area of frame memory where MCU can access. This command makes no change on the
// other driver status. The values of SP[15:0] and EP[15:0] are referred when RAMWR command comes. Each value
// represents one Page line in the Frame Memory.
//
// 1st Parameter SP15:8  start page
// 2nd Parameter SP7:0   start page
// 3rd Parameter EP15:8  end page
// 4th Parameter EP7:0   end page
// 
void lcd_ili9486_page_address_set (uint8_t sp_15_to_8, uint8_t sp_7_to_0, uint8_t ep_15_to_8, uint8_t ep_7_to_0); 

// 8.2.22. Memory Write (2Ch);
//
// This command transfers image data from the host processor to ILI9486’s frame memory starting at the pixel location
// specified by preceding Column Address Set (2Ah); and Page Address Set (2Bh); commands.
// If Memory Access Control (36h); B5 = 0:
// The column and page registers are reset to the Start Column (SC); and Start Page (SP);, respectively. Pixel Data 1 is stored
// in frame memory at (SC, SP);. The column register is then incremented and pixels are written to the frame memory until the
// column register equals the End Column (EC); value. The column register is then reset to SC and the page register is
// incremented. Pixels are written to the frame memory until the page register equals the End Page (EP); value or the host
// processor sends another command. If the number of pixels exceeds (EC – SC + 1); * (EP – SP + 1); the extra pixels are
// ignored.
// If Memory Access control (36h); B5 = 1:
// The column and page registers are reset to the Start Column (SC); and Start Page (SP);, respectively. Pixel Data 1 is stored
// in frame memory at (SC, SP);. The page register is then incremented and pixels are written to the frame memory until the
// page register equals the End Page (EP); value. The page register is then reset to SP and the column register is incremented.
// Pixels are written to the frame memory until the column register equals the End column (EC); value or the host processor
// sends another command. If the number of pixels exceeds (EC – SC + 1); * (EP – SP + 1); the extra pixels are ignored.
//
// 1st Parameter: D1 15:0
// nth Parameter: Dn 15:0
//
void lcd_ili9486_memory_write(uint16_t color, uint32_t total_pixels);

// 8.2.28. Memory Access Control (36h);
//
// This command defines read/write scanning direction of frame memory.
// This command makes no change on the other driver status.
//
// Bit Symbol Name                     Description
// D7  MY     Row Address Order        These 3 bits control MPU to memory write/read direction.
// D6  MX     Column Address Order     These 3 bits control MPU to memory write/read direction.
// D5  MV     Row / Column Exchange    These 3 bits control MPU to memory write/read direction.
// D4  ML     Vertical Refresh Order   LCD vertical refresh direction control.
// D3  BGR    RGB-BGR Order            Color selector switch control (0=RGB color filter panel, 1=BGR color filter panel);
// D2  MH     Horizontal Refresh ORDER LCD horizontal refreshing direction control.
// D1  X      Reserved                 Reserved
// D0  X      Reserved                 Reserved
//
void lcd_ili9486_memory_access_control (uint8_t parameter); 

// 8.2.30. Idle Mode OFF (38h);
//
// This command causes ILI9486 to exit Idle mode.
// In Idle OFF mode, display panel can display maximum 262,144 colors.
//
void lcd_ili9486_idle_mode_off (); 

// 8.2.32. Interface Pixel Format (3Ah);
//
// This command sets the pixel format for the RGB image data used by the interface. DPI[3:0] is the pixel format select of RGB
// interface and DBI[2:0] is the pixel format of CPU interface. If a particular interface, either RGB interface or CPU interface, is
// not used then the corresponding bits in the parameter are ignored. The pixel format are shown in the table below.
//
void lcd_ili9486_interface_pixel_format (uint8_t bits); 


// 8.2.37. Write Display Brightness Value (51h);
//
// This command is used to adjust the brightness value of the display.
// DBV[7:0]: 8 bit, for display brightness of manual brightness setting and CABC in ILI9486. There is a PWM output signal,
// PWM_OUT pin, to control the LED driver IC in order to control display brightness.
// In principle relationship is that 00h value means the lowest brightness and FFh value means the highest brightness.
//
void lcd_ili9486_write_display_brightness_value (uint8_t brightness); 

// 8.2.56. Display Function Control (B6h);
//
void lcd_ili9486_display_function_control (); 

// 8.2.58. Power Control 1 (C0h);
// 
// VRH1[4:0]: Sets the VREG1OUT voltage for positive gamma
// VRH2[4:0]: Sets the VREG2OUT voltage for negative gamma
// 
void lcd_ili9486_power_control_1 ();

// 8.2.59. Power Control 2 (C1h);
//
// BT [2:0]: Sets the factor used in the step-up circuits.
// Select the optimal step-up factor for the operating voltage. To reduce power consumption, set a smaller factor.
// Note: To prevent the device damage, please keep VGH – DDVDH < 8V condition.
// SAP [2:0]: It is used to adjust the constant current in the operational amplifier circuit in the LCD power supply circuit.
// Larger constant current enhances the drivability of the LCD, but it also increases the current consumption. Adjust the
// constant current taking the trade-off between the display quality and the current consumption into account.
// When starting the charge-pump of LCD in the Power ON stage, make sure that SAP=0, and set the SAP=1, after starting
// up the LCD power supply circuit.
// VC [2:0]: Sets VCI1 regulator output voltage.
//
void lcd_ili9486_power_control_2 (); 

// 8.2.60. Power Control 3 (For Normal Mode); (C2h);
//
// DCA0 [2:0]: Selects the operating frequency of the step-up circuit 1/4/5 for Normal mode. The higher step-up operating
// frequency enhances the drivability of the step-up circuit and the quality of display but increases the current consumption.
// Adjust the frequency taking the trade-off between the display quality and the current consumption into account.
// DCA1 [2:0]: Selects the operating frequency of the step-up circuit 2/3 for Normal mode. The higher step-up operating
// frequency enhances the drivability of the step-up circuit and the quality of display but increases the current consumption.
// Adjust the frequency taking the trade-off between the display quality and the current consumption into account.
//
void lcd_ili9486_power_control_3 ();

// 8.2.63. VCOM Control (C5h);
// 
// nVM : When the NV memory is programmed, the nVM will be set 0 : NV memory is not programmed
// as ‘1’ automatically.
// 1 : NV memory is programmed
// VCM_REG [7:0] is used to set factor to generate VCOM voltage from the reference voltage VREG2OUT.
//
void lcd_ili9486_vcom_control (); 

// 8.2.77. PGAMCTRL(Positive Gamma Control); (E0h);
//
void lcd_ili9486_positive_gamma_correction (); 

//
// 8.2.78. NGAMCTRL (Negative Gamma Correction); (E1h);
//
void lcd_ili9486_negative_gamma_correction ();

// Init the LCD
// 
void lcd_ili9486_init ();

#endif