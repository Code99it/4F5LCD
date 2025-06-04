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
void create_menu_frmbuf () {
    printf("[MENU] Creating framebuffer for menu bar, size=%d\n", 29 * LCD_PIXEL_WIDTH * 2);
    uint16_t frmbuf_menubar[29 * LCD_PIXEL_WIDTH * 2];    
}

