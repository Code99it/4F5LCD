#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "gpio.h"
#include "lcd.h"
#include "color.h"

void app_main(void)
{
    printf("Initializing LCD...\n");
    lcd_init();

    printf("Initializing LCD driver (ILI9486)\n");
    lcd_driver_init();

    //printf("Initializing FPS test (Full screen)\n");
	//lcd_test_fill_fps();

    //printf("Initializing FPS test (Partial screen)\n");
    //lcd_test_fill_area_fps(0, 0, 320, 50, 50);

    printf("Filling screen: black\n");
    lcd_fill_color(hex_to_rgb565(0x000000));

    //printf("Filling screen: rainbow colors (10)\n");
	//lcd_fill_rainbow();
    
	uint16_t px_string_end;

	// lcd_draw_horizontal_line(uint16_t y, uint16_t width, uint16_t thickness, uint16_t color)
	printf("Drawing line below the header\n");
	lcd_draw_horizontal_line(61, 320, 2, hex_to_rgb565(0x990000));

	printf("Drawing line above the footer\n");
	lcd_draw_horizontal_line(414, 320, 2, hex_to_rgb565(0x990000));

	printf("Drawing current page title, centered\n");
	px_string_end = lcd_draw_colored_string_centered("Home", 160, 80, 24, 0x0000FF);
	px_string_end = lcd_draw_colored_string("Setup", px_string_end + 13, 80, 24, 0x000078);
	lcd_draw_colored_string("Trip", px_string_end + 13, 80, 24, 0x000042);

	printf("Drawing gear\n");
	px_string_end = lcd_draw_colored_string("D", 145, 430, 29, 0xFFFFFF);
	lcd_draw_colored_string("4", px_string_end, 430, 29, 0X606060);

	printf("Drawing some keys and values\n");
	lcd_draw_colored_string("Outside Temp", 11, 150, 24, 0xa8a8a8);
	lcd_draw_colored_string("Trip Time", 11, 200, 24, 0xa8a8a8);
	lcd_draw_colored_string("Range", 11, 250, 24, 0xa8a8a8);
	lcd_draw_colored_string("AVG Speed", 11, 300, 24, 0xa8a8a8);
	lcd_draw_colored_string("AVG Cons.", 11, 350, 24, 0xa8a8a8);

	lcd_draw_colored_string("+21", 190, 150, 24, 0xFFFFFF);
	lcd_draw_colored_string("01:38", 190, 200, 24, 0xFFFFFF);
	lcd_draw_colored_string("470", 190, 250, 24, 0xFFFFFF);
	lcd_draw_colored_string("56", 190, 300, 24, 0xFFFFFF);
	lcd_draw_colored_string("9,8", 190, 350, 24, 0xFFFFFF);

	lcd_draw_colored_string("°C", 255, 150, 24, 0x404040);
	lcd_draw_colored_string("h:m", 255, 200, 24, 0x404040);
	lcd_draw_colored_string("km", 255, 250, 24, 0x404040);
	lcd_draw_colored_string("km/h", 255, 300, 24, 0x404040);
	lcd_draw_colored_string("l/100", 255, 350, 24, 0x404040);

	printf("Drawing audio source\n");
	lcd_draw_colored_string("Radio Gong 96.3", 11, 17, 24, 0xFF2E12);

    printf("Starting main loop\n");
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
