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

    printf("Initializing FPS test\n");
	lcd_test_fill_fps();

    printf("Filling screen: black\n");
    lcd_fill_color(hex_to_rgb565(0x000000));

    printf("Filling screen: rainbow colors (10)\n");
	lcd_fill_rainbow();
    
    printf("Starting main loop\n");
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
