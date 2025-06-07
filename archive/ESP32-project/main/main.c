#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "pins.h"
#include "gui_config.h"
#include "lcd_init.h"
#include "lcd_driver.h"

void app_main(void)
{
    printf("🛠️  Configuring display via GPIO...\n");

    lcd_init();

    printf("🔍 Heap (internal):    %lu\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("🔍 Heap (SPIRAM):      %lu\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    lcd_fill_screen(0x0000); // Bildschirm schwarz füllen

    uint16_t *framebuffer = heap_caps_malloc(LCD_PX_WIDTH * LCD_PX_HEIGHT * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    if (framebuffer) {
        for (int i = 0; i < LCD_PX_WIDTH * LCD_PX_HEIGHT; i++) {
            framebuffer[i] = 0xF800; // rot
        }

        lcd_draw_bitmap(0, 0, LCD_PX_WIDTH - 1, LCD_PX_HEIGHT - 1, framebuffer);
        free(framebuffer);
        printf("✅ Red screen drawn.\n");
    } else {
        printf("❌ Framebuffer allocation failed.\n");
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
