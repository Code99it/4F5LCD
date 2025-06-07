#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "gpio.h"
#include "lcd-drv-ILI9486.h"

void app_main(void)
{
	
    lcd_init();
    lcd_read_id();
    lcd_run_diagnose();
    lcd_run_full_register_scan();

	lcd_write_command(0x2C);  // Memory Write

	for (uint8_t i = 0; i < 255; i++) {
	    uint16_t color = ((i & 0xF8) << 8);  // R=i, G=0, B=0 → RGB565: R5G6B5
	    lcd_write_rgb565(color);
	    vTaskDelay(pdMS_TO_TICKS(10));
	}

	printf("Starting main loop\n");
    while(1) vTaskDelay(pdMS_TO_TICKS(1000));

    /*

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

	printf("Drawing audio source\n");
	px_string_end = lcd_draw_colored_string("CD1", 11, 17, 24, 0xFF2E12);
	lcd_draw_colored_string("Thank You For The Music", px_string_end + 7, 17, 24, 0x505050);




	char *menu_items[] = {
		"BC1",
		"BC2",
		"BC3",
		"Suspension",
		"Automatic Transmission",
		"Contacts",
		"Radio",
		"Check",
		"Battery"
	};


	int framebuffer_width = LCD_PIXEL_WIDTH * 2;
	int framebuffer_height = 24;

	size_t framebuffer_size = framebuffer_width * framebuffer_height * sizeof(uint16_t);

	uint16_t *frmbuf_menubar = malloc(framebuffer_size);
	if (!frmbuf_menubar) {
	    printf("Fehler: Nicht genügend RAM für Framebuffer menubar\n");
	} else {
	    printf("Framebuffer für die gesamte Menübar, size=%d Bytes\n", (int)framebuffer_size);
	    printf("Framebuffer, width=%d Bytes\n", (int)framebuffer_width);
	    printf("Framebuffer, height=%d Bytes\n", (int)framebuffer_height);
	}

	// Framebuffer komplett mit Farbe auffüllen
	for (int i = 0; i < framebuffer_width * framebuffer_height; i++) {
		frmbuf_menubar[i] = hex_to_rgb565(0x000000); // Schwarz
	}



	// Menüpunkt 0 zentriert anzeigen
	uint16_t centered_menu_item_end_x = lcd_draw_colored_string_centered_at_x_to_frmbuf(
	    frmbuf_menubar,
	    framebuffer_width,
	    menu_items[3],  
	    LCD_PIXEL_WIDTH / 2,       
	    0,
	    13,
	    0x3232FF   
	);
    printf("Endpunkt des Mittigen Strings = %d px\n", (int)centered_menu_item_end_x);

	// Menüpunkt 1 direkt rechts daneben (nicht zentriert!)
	uint16_t second_menu_item_end_x = lcd_draw_colored_string_to_frmbuf(
	    frmbuf_menubar,
	    framebuffer_width,
	    menu_items[4],    
	    centered_menu_item_end_x + MENU_ITEM_SPACING_PX,  // Mit kleinem Abstand anschließen
	    0,
	    13,
	    0x404040
	);	

	uint16_t third_menu_item_end_x = lcd_draw_colored_string_to_frmbuf(
	    frmbuf_menubar,
	    framebuffer_width,
	    menu_items[5],    
	    second_menu_item_end_x + MENU_ITEM_SPACING_PX,  // Mit kleinem Abstand anschließen
	    0,
	    13,
	    0x404040
	);
	
	lcd_draw_colored_string_to_frmbuf(
	    frmbuf_menubar,
	    framebuffer_width,
	    menu_items[6],    
	    third_menu_item_end_x + MENU_ITEM_SPACING_PX,  // Mit kleinem Abstand anschließen
	    0,
	    13,
	    0x404040
	);



	lcd_draw_framebuffer_region(   
	    frmbuf_menubar,        // Zeiger auf den vollständigen Framebuffer (doppelte LCD-Breite)  
	    framebuffer_width,     // Tatsächliche Breite des Framebuffers in Pixeln (z. B. LCD_PIXEL_WIDTH * 2)  
	    0,                     // X-Versatz im Framebuffer: beginnt bei 0, also linke Seite anzeigen  
	    0,                     // Y-Versatz im Framebuffer: beginnt bei oberster Zeile  
	    LCD_PIXEL_WIDTH,       // Breite des anzuzeigenden Ausschnitts (exakt LCD-Breite)  
	    framebuffer_height,    // Höhe des Ausschnitts in Pixeln (z. B. 24px für Menübar)  
	    0,                     // X-Position auf dem LCD, an der der Ausschnitt dargestellt wird  
	    96                     // Y-Position auf dem LCD (z. B. vertikal unterhalb eines Headers)  
	);




	lcd_draw_centered_rounded_frame(
	    menu_items[3],      // Textstring (z. B. "BC1"), um den der Rahmen gezeichnet wird
	    160,                // Mittelpunkt auf dem LCD in X-Richtung (horizontal zentriert)
	    95,                  // Y-Position des oberen Rahmens (Startpunkt vertikal)
	    13,                 // Schriftgröße in Pixelhöhe (z. B. 13px)
	    5,                  // Innenabstand (Padding) zwischen Text und Rahmen in Pixeln
	    2,                  // Rahmenbreite (Thickness) in Pixeln
	    5,                  // Radius der gerundeten Ecken in Pixeln
	    0x2424FF            // Rahmenfarbe in Hex 
	);


	vTaskDelay(pdMS_TO_TICKS(3000));

	// Rahmen entfernen
	lcd_delete_centered_rounded_frame(
	    menu_items[3],      // Textinhalt
	    160,                // Zentrierung X
	    95,                 // Position Y
	    13,                 // Schriftgröße
	    5,                  // Padding
	    2,                  // Rahmenbreite
	    5,                  // Eckenradius
	    0x000000            // Hintergrundfarbe
	);

	// Entfärbten aktuellen Menüpunkt zeigen
	// Menüpunkt 0 zentriert anzeigen
	lcd_draw_colored_string_centered_at_x_to_frmbuf(
	    frmbuf_menubar,
	    framebuffer_width,
	    menu_items[3],  
	    LCD_PIXEL_WIDTH / 2,       
	    0,
	    13,
	    0x404040   
	);

	// in einer schleife zum nächsten menüpunkt gehen
	// framebuffer immer wieder neu zeichnen
	for (uint8_t left_cut=0; left_cut<189; left_cut++) {
		lcd_draw_framebuffer_region(   
		    frmbuf_menubar,        // Zeiger auf den vollständigen Framebuffer (doppelte LCD-Breite)  
		    framebuffer_width,     // Tatsächliche Breite des Framebuffers in Pixeln (z. B. LCD_PIXEL_WIDTH * 2)  
		    left_cut,              // X-Versatz im Framebuffer: beginnt bei 0, also linke Seite anzeigen  
		    0,                     // Y-Versatz im Framebuffer: beginnt bei oberster Zeile  
		    LCD_PIXEL_WIDTH,       // Breite des anzuzeigenden Ausschnitts (exakt LCD-Breite)  
		    framebuffer_height,    // Höhe des Ausschnitts in Pixeln (z. B. 24px für Menübar)  
		    0,                     // X-Position auf dem LCD, an der der Ausschnitt dargestellt wird  
		    96                     // Y-Position auf dem LCD (z. B. vertikal unterhalb eines Headers)  
		);
		//vTaskDelay(pdMS_TO_TICKS(2));
	}

	lcd_draw_centered_rounded_frame(
	    menu_items[4],      // Textstring (z.B. "BC1"), um den der Rahmen gezeichnet wird
	    160,                // Mittelpunkt auf dem LCD in X-Richtung (horizontal zentriert)
	    95,                  // Y-Position des oberen Rahmens (Startpunkt vertikal)
	    13,                 // Schriftgröße in Pixelhöhe (z. B. 13px)
	    5,                  // Innenabstand (Padding) zwischen Text und Rahmen in Pixeln
	    2,                  // Rahmenbreite (Thickness) in Pixeln
	    5,                  // Radius der gerundeten Ecken in Pixeln
	    0x2424FF            // Rahmenfarbe in Hex 
	);


	printf("Drawing some keys and values\n");
	lcd_draw_colored_string("D: shift up +", 51, 150, 24, 0xa8a8a8);
	lcd_draw_colored_string("D: shift dn -", 51, 190, 24, 0xa8a8a8);
	lcd_draw_colored_string("S: shift up +", 51, 230, 24, 0xa8a8a8);
	lcd_draw_colored_string("S: shift dn -", 51, 270, 24, 0xa8a8a8);
	lcd_draw_colored_string("M: shift up +", 51, 310, 24, 0xa8a8a8);
	lcd_draw_colored_string("M: shift dn -", 51, 350, 24, 0xa8a8a8);

	lcd_draw_colored_string("3100", 248, 150, 24, 0xFFFFFF);
	lcd_draw_colored_string("1400", 248, 190, 24, 0xFFFFFF);
	lcd_draw_colored_string("4000", 248, 230, 24, 0xFFFFFF);
	lcd_draw_colored_string("1900", 248, 270, 24, 0xFFFFFF);
	lcd_draw_colored_string("4500", 248, 310, 24, 0xFFFFFF);
	lcd_draw_colored_string("1600", 248, 350, 24, 0xFFFFFF);








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
	*/
}
