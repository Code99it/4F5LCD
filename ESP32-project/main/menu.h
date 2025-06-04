// menu.h
#pragma once

#include <stdint.h>

// Sprache wählen (Deutsch als Beispiel)
enum Language {
    LANGUAGE_DE,
    LANGUAGE_EN,
    LANGUAGE_PL
};
extern int current_language;

// Anzahl der Menüeinträge
#define MENUITEM_COUNT 10

// Abstand vom oberen LCD Rand
#define MENUITEM_MARGIN_TOP 80

void menu_spawn_frmbuf();
void menu_init();
