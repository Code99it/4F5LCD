#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "soc/gpio_struct.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "Code99-DRV-LCD-ILI9486-8-wire-parallel.h"

// Code99 LCD Driver for ILI9486 - ESP32

// For 8-bit BUS interface D0 - D7

// Set Data Mask (für GPIOs > 31)
#define LCD_DATA_MASK ( \
    (1U << (GPIO_NUM_35 - 32)) | \
    (1U << (GPIO_NUM_36 - 32)) | \
    (1U << (GPIO_NUM_37 - 32)) | \
    (1U << (GPIO_NUM_38 - 32)) | \
    (1U << (GPIO_NUM_39 - 32)) | \
    (1U << (GPIO_NUM_40 - 32)) | \
    (1U << (GPIO_NUM_41 - 32)) | \
    (1U << (GPIO_NUM_42 - 32))   \
)

// Set Data Pins
void IRAM_ATTR lcd_ili9486_data (uint8_t data) {
    if (data & 0x01) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_35 - 32));
    if (data & 0x02) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_36 - 32));
    if (data & 0x04) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_37 - 32));
    if (data & 0x08) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_38 - 32));
    if (data & 0x10) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_39 - 32));
    if (data & 0x20) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_40 - 32));
    if (data & 0x40) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_41 - 32));
    if (data & 0x80) GPIO.out1_w1ts.val = (1U << (GPIO_NUM_42 - 32));
}

// GPIO Pin control for WRX (WR = Write Strobe)
//
void lcd_ili9486_pin_wrx (bool state) {
	gpio_set_level(LCD_WR, state);
}

// GPIO Pin control for DCX (RS = Register Select)
//
void lcd_ili9486_pin_dcx (bool state) {
	gpio_set_level(LCD_RS, state);
}

// GPIO Pin control for RDX (RD = Read)
//
void lcd_ili9486_pin_rdx (bool state) {
	gpio_set_level(LCD_RD, state);
}

// GPIO Pin control for CS (Chip select)
//
void lcd_ili9486_pin_cs (bool state) {
	gpio_set_level(LCD_CS, state);
}

// GPIO Pin control for RST (Reset)
//
void lcd_ili9486_pin_rst (bool state) {
	gpio_set_level(LCD_RST, state);
}

// A short pulse on the WRX pin (WR = Write Strobe)
//
void lcd_ili9486_write_strobe () {
	lcd_ili9486_pin_wrx(LOW);
	esp_rom_delay_us(10);
	lcd_ili9486_pin_wrx(HIGH);
}

// Hard reset
//
void lcd_ili9486_hard_reset () {
    printf("LCD hard reset\n");
    gpio_set_direction(LCD_RST, GPIO_MODE_OUTPUT);
    lcd_ili9486_pin_rst(HIGH);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_ili9486_pin_rst(LOW);
    vTaskDelay(pdMS_TO_TICKS(50));
    lcd_ili9486_pin_rst(HIGH);
    vTaskDelay(pdMS_TO_TICKS(120));
}


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
// S/W Reset default values. (See default tables in each command description.)
// The display is blank immediately
// Note: The Frame Memory contents is kept or not by this command.
// It will be necessary to wait 5msec before sending new command following software reset. The display module loads all
// display supplier factory default values to the registers during this 5msec. If Software Reset is applied during Sleep Out
// mode, it will be necessary to wait 120msec before sending Sleep out command. Software Reset Command cannot be sent
// during Sleep Out sequence.
//
void lcd_ili9486_soft_reset () {
	printf("LCD soft reset\n");
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00000001);
	lcd_ili9486_write_strobe();
	vTaskDelay(pdMS_TO_TICKS(5));
}

// 8.2.15. Normal Display Mode ON (13h)
//
// This command returns the display to normal mode. Normal display mode on means Partial mode off and Scroll mode off.
//
void lcd_ili9486_normal_display_mode_on () {
	printf("LCD normal display mode on\n");
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00010011);
	lcd_ili9486_write_strobe();
}


// 8.2.19. Display ON (29h)
//
// This command causes ILI9486 to start displaying the image data on the display device. The frame memory contents remain
// unchanged. No status bits are changed.
//
void lcd_ili9486_display_on () {
	printf("LCD display on\n");
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00101001);
	lcd_ili9486_write_strobe();
}

// 8.2.20. Column Address Set (2Ah)
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
void lcd_ili9486_column_address_set (uint8_t sc_15_to_8, uint8_t sc_7_to_0, uint8_t ec_15_to_8, uint8_t ec_7_to_0) {
    uint16_t start = (sc_15_to_8 << 8) | sc_7_to_0;
    uint16_t end   = (ec_15_to_8 << 8) | ec_7_to_0;
    printf("LCD column address set from %u to %u\n", start, end);
   	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00101010);
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);
	lcd_ili9486_data(sc_15_to_8);
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(sc_7_to_0);
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(ec_15_to_8);
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(ec_7_to_0);
	lcd_ili9486_write_strobe();
}

// 8.2.21. Page Address Set (2Bh)
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
void lcd_ili9486_page_address_set (uint8_t sp_15_to_8, uint8_t sp_7_to_0, uint8_t ep_15_to_8, uint8_t ep_7_to_0) {
    uint16_t start = (sp_15_to_8 << 8) | sp_7_to_0;
    uint16_t end   = (ep_15_to_8 << 8) | ep_7_to_0;
    printf("LCD page address set from %u to %u\n", start, end);
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00101011);
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);
	lcd_ili9486_data(sp_15_to_8);
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(sp_7_to_0);
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(ep_15_to_8);
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(ep_7_to_0);
	lcd_ili9486_write_strobe();
}

// 8.2.22. Memory Write (2Ch)
//
// This command transfers image data from the host processor to ILI9486’s frame memory starting at the pixel location
// specified by preceding Column Address Set (2Ah) and Page Address Set (2Bh) commands.
// If Memory Access Control (36h) B5 = 0:
// The column and page registers are reset to the Start Column (SC) and Start Page (SP), respectively. Pixel Data 1 is stored
// in frame memory at (SC, SP). The column register is then incremented and pixels are written to the frame memory until the
// column register equals the End Column (EC) value. The column register is then reset to SC and the page register is
// incremented. Pixels are written to the frame memory until the page register equals the End Page (EP) value or the host
// processor sends another command. If the number of pixels exceeds (EC – SC + 1) * (EP – SP + 1) the extra pixels are
// ignored.
// If Memory Access control (36h) B5 = 1:
// The column and page registers are reset to the Start Column (SC) and Start Page (SP), respectively. Pixel Data 1 is stored
// in frame memory at (SC, SP). The page register is then incremented and pixels are written to the frame memory until the
// page register equals the End Page (EP) value. The page register is then reset to SP and the column register is incremented.
// Pixels are written to the frame memory until the column register equals the End column (EC) value or the host processor
// sends another command. If the number of pixels exceeds (EC – SC + 1) * (EP – SP + 1) the extra pixels are ignored.
//
// 1st Parameter: D1 15:0
// nth Parameter: Dn 15:0
//
void lcd_ili9486_memory_write(uint16_t color, uint32_t total_pixels) {
    lcd_ili9486_pin_dcx(LOW);
    lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00101100); 
    lcd_ili9486_write_strobe();
    lcd_ili9486_pin_dcx(HIGH);

    uint8_t color_high_byte = (color >> 8) & 0xFF;
    uint8_t color_low_byte  = color & 0xFF;

    for (uint32_t i = 0; i < total_pixels; i++) {
        lcd_ili9486_data(color_high_byte); lcd_ili9486_write_strobe();
        lcd_ili9486_data(color_low_byte);  lcd_ili9486_write_strobe();
    }
}

// 8.2.28. Memory Access Control (36h)
//
// This command defines read/write scanning direction of frame memory.
// This command makes no change on the other driver status.
//
// Bit Symbol Name                     Description
// D7  MY     Row Address Order        These 3 bits control MPU to memory write/read direction.
// D6  MX     Column Address Order     These 3 bits control MPU to memory write/read direction.
// D5  MV     Row / Column Exchange    These 3 bits control MPU to memory write/read direction.
// D4  ML     Vertical Refresh Order   LCD vertical refresh direction control.
// D3  BGR    RGB-BGR Order            Color selector switch control (0=RGB color filter panel, 1=BGR color filter panel)
// D2  MH     Horizontal Refresh ORDER LCD horizontal refreshing direction control.
// D1  X      Reserved                 Reserved
// D0  X      Reserved                 Reserved
//
void lcd_ili9486_memory_access_control (uint8_t parameter) {
    printf("LCD memory access control set to 0b");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (parameter >> i) & 1);
    }
    printf("\n");
   	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00110110); 
	lcd_ili9486_write_strobe();	
	lcd_ili9486_pin_dcx(HIGH);
	lcd_ili9486_data(parameter); 
}

// 8.2.30. Idle Mode OFF (38h)
//
// This command causes ILI9486 to exit Idle mode.
// In Idle OFF mode, display panel can display maximum 262,144 colors.
//
void lcd_ili9486_idle_mode_off () {
    printf("LCD idle mode off\n");
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00111000); 
	lcd_ili9486_write_strobe();	
}

// 8.2.32. Interface Pixel Format (3Ah)
//
// This command sets the pixel format for the RGB image data used by the interface. DPI[3:0] is the pixel format select of RGB
// interface and DBI[2:0] is the pixel format of CPU interface. If a particular interface, either RGB interface or CPU interface, is
// not used then the corresponding bits in the parameter are ignored. The pixel format are shown in the table below.
//
void lcd_ili9486_interface_pixel_format (uint8_t bits) {
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b00111010); 
	lcd_ili9486_write_strobe();	
	printf("LCD interface pixel format set to %u\n", bits);
	switch (bits) {
		case 16:
			lcd_ili9486_data(0b01010101); 
		break;
		case 18:
			lcd_ili9486_data(0b01100110); 
		break;
	}
	lcd_ili9486_write_strobe();	
}


// 8.2.37. Write Display Brightness Value (51h)
//
// This command is used to adjust the brightness value of the display.
// DBV[7:0]: 8 bit, for display brightness of manual brightness setting and CABC in ILI9486. There is a PWM output signal,
// PWM_OUT pin, to control the LED driver IC in order to control display brightness.
// In principle relationship is that 00h value means the lowest brightness and FFh value means the highest brightness.
//
void lcd_ili9486_write_display_brightness_value (uint8_t brightness) {
	printf("LCD write display brightness value = %u\n", brightness);
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b01010001); 
	lcd_ili9486_write_strobe();		
	lcd_ili9486_pin_dcx(HIGH);
	lcd_ili9486_data(brightness); 
	lcd_ili9486_write_strobe();		
}

// 8.2.56. Display Function Control (B6h)
//
void lcd_ili9486_display_function_control () {
	printf("LCD display function control\n");
	lcd_ili9486_pin_dcx(LOW);
	lcd_ili9486_pin_rdx(HIGH);
	lcd_ili9486_data(0b10110110); 
	lcd_ili9486_write_strobe();		
	lcd_ili9486_pin_dcx(HIGH);
	lcd_ili9486_data(0b00001010); 
	lcd_ili9486_write_strobe();		
	lcd_ili9486_data(0b10100010); 
	lcd_ili9486_write_strobe();		
	lcd_ili9486_data(0b00000100); 
	lcd_ili9486_write_strobe();		
}

// 8.2.58. Power Control 1 (C0h)
// 
// VRH1[4:0]: Sets the VREG1OUT voltage for positive gamma
// VRH2[4:0]: Sets the VREG2OUT voltage for negative gamma
// 
void lcd_ili9486_power_control_1 () {
	printf("LCD display power control 1\n");
	lcd_ili9486_pin_dcx(LOW);              
	lcd_ili9486_pin_rdx(HIGH);             
	lcd_ili9486_data(0b11000000);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);             
	lcd_ili9486_data(0b00001101);         
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001101);          
	lcd_ili9486_write_strobe();
}

// 8.2.59. Power Control 2 (C1h)
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
void lcd_ili9486_power_control_2 () { 
	printf("LCD display power control 2\n");
	lcd_ili9486_pin_dcx(LOW);              
	lcd_ili9486_pin_rdx(HIGH);            
	lcd_ili9486_data(0b11000001);         
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);             
	lcd_ili9486_data(0b10101010);          
	lcd_ili9486_write_strobe();
}

// 8.2.60. Power Control 3 (For Normal Mode) (C2h)
//
// DCA0 [2:0]: Selects the operating frequency of the step-up circuit 1/4/5 for Normal mode. The higher step-up operating
// frequency enhances the drivability of the step-up circuit and the quality of display but increases the current consumption.
// Adjust the frequency taking the trade-off between the display quality and the current consumption into account.
// DCA1 [2:0]: Selects the operating frequency of the step-up circuit 2/3 for Normal mode. The higher step-up operating
// frequency enhances the drivability of the step-up circuit and the quality of display but increases the current consumption.
// Adjust the frequency taking the trade-off between the display quality and the current consumption into account.
//
void lcd_ili9486_power_control_3 () {
	printf("LCD display power control 3\n");
	lcd_ili9486_pin_dcx(LOW);              
	lcd_ili9486_pin_rdx(HIGH);            
	lcd_ili9486_data(0b11000010);         	
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);             
	lcd_ili9486_data(0b00101010);          
	lcd_ili9486_write_strobe();
}

// 8.2.63. VCOM Control (C5h)
// 
// nVM : When the NV memory is programmed, the nVM will be set 0 : NV memory is not programmed
// as ‘1’ automatically.
// 1 : NV memory is programmed
// VCM_REG [7:0] is used to set factor to generate VCOM voltage from the reference voltage VREG2OUT.
//
void lcd_ili9486_vcom_control () {
	printf("LCD display vom control\n");
	lcd_ili9486_pin_dcx(LOW);              
	lcd_ili9486_pin_rdx(HIGH);            
	lcd_ili9486_data(0b11000101);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);             
	lcd_ili9486_data(0b00001010);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001010);          
	lcd_ili9486_write_strobe();
}

// 8.2.77. PGAMCTRL(Positive Gamma Control) (E0h)
//
void lcd_ili9486_positive_gamma_correction () {
	printf("LCD positive gamma correction\n");
	lcd_ili9486_pin_dcx(LOW);              
	lcd_ili9486_pin_rdx(HIGH);            
	lcd_ili9486_data(0b11100000);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);             
	lcd_ili9486_data(0b00001111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00011111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00011100);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001100);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001000);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b01001000);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b10011000);         
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00110111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001010);         
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00010011);         
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00000100);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00010001);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001101);         
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00000000);          
	lcd_ili9486_write_strobe();
}

//
// 8.2.78. NGAMCTRL (Negative Gamma Correction) (E1h)
//
void lcd_ili9486_negative_gamma_correction () {
	printf("LCD negative gamma correction\n");
	lcd_ili9486_pin_dcx(LOW);              
	lcd_ili9486_pin_rdx(HIGH);            
	lcd_ili9486_data(0b11100001);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_pin_dcx(HIGH);             
	lcd_ili9486_data(0b00001111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00011111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00011100);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001100);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001000);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b01001000);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b10011000);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00110111);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001010);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00010011);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00000100);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00010001);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00001101);          
	lcd_ili9486_write_strobe();
	lcd_ili9486_data(0b00000000);          
	lcd_ili9486_write_strobe();
}

// Init the LCD
// 
void lcd_ili9486_init () {
	lcd_ili9486_hard_reset();
	lcd_ili9486_pin_cs(HIGH);               
	lcd_ili9486_soft_reset();               
	lcd_ili9486_memory_access_control(0b01101100);
	lcd_ili9486_interface_pixel_format(16);   
	lcd_ili9486_display_function_control(); 
	lcd_ili9486_idle_mode_off();           
	lcd_ili9486_normal_display_mode_on();  
	lcd_ili9486_display_on();              
}




















