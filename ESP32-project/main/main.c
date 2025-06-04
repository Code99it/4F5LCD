#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "gpio.h"
#include "lcd.h"
#include "color.h"
#include "menu.h"

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
	lcd_draw_horizontal_line(61, 320, 3, hex_to_rgb565(0x990000));

	printf("Drawing line above the footer\n");
	lcd_draw_horizontal_line(414, 320, 3, hex_to_rgb565(0x990000));

	printf("Drawing gear\n");
	px_string_end = lcd_draw_colored_string("D", 145, 430, 29, 0xFFFFFF);
	lcd_draw_colored_string("4", px_string_end, 430, 29, 0X606060);

	printf("Drawing some keys and values\n");
	lcd_draw_colored_string("Out Temp", 55, 150, 24, 0xa8a8a8);
	lcd_draw_colored_string("Trip Time", 55, 190, 24, 0xa8a8a8);
	lcd_draw_colored_string("Range", 55, 230, 24, 0xa8a8a8);
	lcd_draw_colored_string("AVG Spd", 55, 270, 24, 0xa8a8a8);
	lcd_draw_colored_string("AVG Cons", 55, 310, 24, 0xa8a8a8);
	lcd_draw_colored_string("Oil Temp", 55, 350, 24, 0xa8a8a8);

	lcd_draw_colored_string("+21", 190, 150, 24, 0xFFFFFF);
	lcd_draw_colored_string("01:38", 190, 190, 24, 0xFFFFFF);
	lcd_draw_colored_string("470", 190, 230, 24, 0xFFFFFF);
	lcd_draw_colored_string("56", 190, 270, 24, 0xFFFFFF);
	lcd_draw_colored_string("9,8", 190, 310, 24, 0xFFFFFF);
	lcd_draw_colored_string("61", 190, 350, 24, 0xFFFFFF);

	lcd_draw_colored_string("°C", 255, 150, 24, 0x404040);
	lcd_draw_colored_string("h:m", 255, 190, 24, 0x404040);
	lcd_draw_colored_string("km", 255, 230, 24, 0x404040);
	lcd_draw_colored_string("km/h", 255, 270, 24, 0x404040);
	lcd_draw_colored_string("l/100", 255, 310, 24, 0x404040);
	lcd_draw_colored_string("°C", 255, 350, 24, 0x404040);

	printf("Drawing audio source\n");
	px_string_end = lcd_draw_colored_string("CD1", 11, 17, 24, 0xFF2E12);
	lcd_draw_colored_string("Thank You For The Music", px_string_end + 7, 17, 24, 0x505050);



	//menu_init();



	// Alle Parameter definieren
	uint16_t ring_diameter = 24;
	uint16_t ring_thickness = 3;
	uint16_t center_diameter = 14;
	uint16_t line_length = 16;
	uint16_t line_thickness = 2;
	uint16_t padding = 0;

	// Berechnung der Framebuffergröße
	uint16_t fb_width = ring_diameter + line_length + 2 + padding * 2;
	uint16_t fb_height = ring_diameter + padding * 2;

	// Dynamischen Speicher allokieren
	uint16_t *select_fb = malloc(fb_width * fb_height * sizeof(uint16_t));
	if (!select_fb) {
	    printf("Fehler: Nicht genügend RAM für Framebuffer\n");
	    return;
	}

	// Framebuffer löschen
	memset(select_fb, 0, fb_width * fb_height * sizeof(uint16_t));

    printf("Starting main loop\n");
    printf("Quit monitor: Control+] (Control+Option+6)\n");

    while (true)
    {

		for (uint16_t pointerpos=152; pointerpos <=352; pointerpos+=40) {

			// Draw Line Selector into frame buffer
			lcd_draw_menu_select_point(
			    select_fb,
			    fb_width,
			    fb_height,
			    padding,       // x_offset
			    padding,       // y_offset
			    ring_diameter,
			    ring_thickness,
			    center_diameter,
			    0xFF0000,      // ring_color
			    0xA0A0A0,      // center_color
			    0xFF0000,      // line_color
			    line_length,
			    line_thickness,
			    0x000000       // background (schwarz)
			);

			// Transmit Frame Buffer to LCD
			lcd_blit_framebuffer(select_fb, fb_width, fb_height, 7, pointerpos);

			// Wait...
			vTaskDelay(pdMS_TO_TICKS(1000));

			// Fill Framebuffer black
			lcd_fill_framebuffer(select_fb, fb_width, fb_height, 0x000000);  

			// Transmit Frame Buffer to LCD
			lcd_blit_framebuffer(select_fb, fb_width, fb_height, 7, pointerpos);
		}

    }

    // Speicher freigeben
	free(select_fb);

}
