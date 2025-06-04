// menu.c
#include "menu.h"
#include <string.h>
#include <stdio.h>
#include "font.h"
#include "lcd.h"
#include "color.h"

// Menü-Sprache (Definition)
int current_language = LANGUAGE_DE;

// Der aktuell gewählte und sichtbare Menü-Eintrag
int current_menu_page = 0;

// Menüeinträge
const char *menuitems[3][MENUITEM_COUNT] = {
    { "BC1", "BC2", "Setup",      "Trip",  "Rennen", "Runde",     "Turbo", "GPS", "Check",    "Telefon" },
    { "BC1", "BC2", "Setup",      "Trip",  "Race",   "Lap",       "Turbo", "GPS", "Check",    "Phone"   },
    { "BC1", "BC2", "Ustawienia", "Trasa", "Wyścig", "Okrążenie", "Turbo", "GPS", "Kontrola", "Telefon" }
};

// Framebuffer für die gesamte Menüzeile plus nochmal die Displaybreite dazu damit genug "Fleisch" für linksslide da ist
void menu_spawn_frmbuf() {
    printf("[MENU] Creating framebuffer for menu bar, size=%d\n", 29 * LCD_PIXEL_WIDTH * 2);
    uint16_t frmbuf_menubar[29 * LCD_PIXEL_WIDTH * 2];    
}

// Finde die Pixel-Mitte des aktuell gewählten Menü-Eintrags
/*uint16_t find_center_of_string(current_menu_page) {
    uint16_t px;
    printf("[MENU] Finding center of current menu page string, left-to-center=%d\n", px);
    return px;
}*/

// Write current menu page into frame buffer
void write_current_menu_page_in_frmbuf () {
    uint16_t framebuf_x_pos = LCD_PIXEL_WIDTH / 2;
    //lcd_draw_colored_string_centered(const char *str,              uint16_t x_center, uint16_t y,          uint8_t px, uint32_t hex_rgb)
      //lcd_draw_colored_string_centered(menuitems[current_menu_page], framebuf_x_pos,    MENUITEM_MARGIN_TOP, 24,         0XFFFFFF);
}

// Draw a section of the framebuffer to LCD
void draw_frmbuf_section () {
    //frmbuf_menubar
}

void menu_init() {
    //menu_spawn_frmbuf();
}