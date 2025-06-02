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
    
	printf("Drawing Letter\n");
	uint16_t next_x = lcd_draw_char('Q', 20, 120, 36);

	printf("Drawing String\n");
	uint16_t end_x = lcd_draw_string("QQQQQQQQQQ", 20, 170, 29);
	
	// lcd_draw_horizontal_line(uint16_t y, uint16_t width, uint16_t thickness, uint16_t color)
	printf("Drawing line below the header\n");
	lcd_draw_horizontal_line(55, 320, 2, hex_to_rgb565(0x990000));

	printf("Drawing line above the footer\n");
	lcd_draw_horizontal_line(420, 320, 2, hex_to_rgb565(0x990000));

    printf("Starting main loop\n");
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
