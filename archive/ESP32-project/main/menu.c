// menu.c
#include <stdio.h>
#include "menu.h"
#include "lcd_driver.h"
#include "gui_config.h"
#include "font_PT_Sans_24px_white.h"

// Verfügbare Menüseiten
// Einträge und Reihenfolge der Seiten nur hier ändern
// ********************************************************************************************************************
const char *gui_menu_pages[] = {
    "Home",
    "Setup",
    "Radio",
    "Media",
    "Race",
    "Bluetooth",
    "DPF",
    "CD",
    "Reifendruck",
    "Telefon",
    "Füllstände",
    "Temperaturen",
    "Luftfahrwerk",
    "GPS",
    "Motor",
    "TV",
    "Getriebe",
    "Einparkhilfe",
    "iRDKS",
    "Reichweite",
    "Wegfahrsperre",
    "Licht",
    "Fahrzeit",
    "Audio",
    "CHECK"
};
const int gui_menu_page_count = sizeof(gui_menu_pages) / sizeof(gui_menu_pages[0]);
// ********************************************************************************************************************



// Einen bestimmten Menütitel im Footer anzeigen
// ********************************************************************************************************************
void draw_footer_page_title(const char *page_title)
{
    int page_string_width = lcd_i80_measure_string(page_title);
    int page_string_center = page_string_width / 2;

    int x_footer = LCD_PX_WIDTH / 2 - page_string_center;
    int y_footer = LCD_PX_HEIGHT - GUI_BORDER_BOTTOM - GUI_FOOTER_MARGIN_BOTTOM - GUI_FONT_1_HEIGHT;

    printf(
        "GUI: Footer-Titel wird geschrieben, Start x=%d, y=%d, Breite=%d Pixel, Text=\"%s\"\n",
        x_footer, y_footer, page_string_width, page_title
    );

    lcd_i80_write_string(x_footer, y_footer, page_title);
}
// ********************************************************************************************************************
