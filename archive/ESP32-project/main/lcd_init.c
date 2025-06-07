#include "lcd_init.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "esp_log.h"

static const char *TAG = "lcd_gpio";

// ⚙️ Helper Macros
#define LCD_CS_LOW()     gpio_set_level(LCD_CS, 0)
#define LCD_CS_HIGH()    gpio_set_level(LCD_CS, 1)
#define LCD_RS_LOW()     gpio_set_level(LCD_RS, 0)
#define LCD_RS_HIGH()    gpio_set_level(LCD_RS, 1)
#define LCD_WR_LOW()     gpio_set_level(LCD_WR, 0)
#define LCD_WR_HIGH()    gpio_set_level(LCD_WR, 1)

#define WRITE_BYTE(val)  write_8bit(val)

static void write_8bit(uint8_t val)
{
    gpio_set_level(LCD_DATA_0, (val >> 0) & 1);
    gpio_set_level(LCD_DATA_1, (val >> 1) & 1);
    gpio_set_level(LCD_DATA_2, (val >> 2) & 1);
    gpio_set_level(LCD_DATA_3, (val >> 3) & 1);
    gpio_set_level(LCD_DATA_4, (val >> 4) & 1);
    gpio_set_level(LCD_DATA_5, (val >> 5) & 1);
    gpio_set_level(LCD_DATA_6, (val >> 6) & 1);
    gpio_set_level(LCD_DATA_7, (val >> 7) & 1);

    LCD_WR_LOW();
    __asm__ __volatile__("nop;nop;nop;nop;nop;");
    LCD_WR_HIGH();
}

void lcd_send_command(uint8_t cmd)
{
    LCD_CS_LOW();
    LCD_RS_LOW();    // Command-Modus
    WRITE_BYTE(cmd);
    LCD_CS_HIGH();
}

void lcd_send_data(uint8_t data)
{
    LCD_CS_LOW();
    LCD_RS_HIGH();   // Daten-Modus
    WRITE_BYTE(data);
    LCD_CS_HIGH();
}

void lcd_send_data16(uint16_t data)
{
    lcd_send_data(data >> 8);
    lcd_send_data(data & 0xFF);
}

void lcd_reset_display(void)
{
    gpio_set_level(LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(150));
}

void lcd_gpio_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LCD_CS) | (1ULL << LCD_RS) | (1ULL << LCD_WR) | (1ULL << LCD_RST) |
                        (1ULL << LCD_DATA_0) | (1ULL << LCD_DATA_1) | (1ULL << LCD_DATA_2) | (1ULL << LCD_DATA_3) |
                        (1ULL << LCD_DATA_4) | (1ULL << LCD_DATA_5) | (1ULL << LCD_DATA_6) | (1ULL << LCD_DATA_7),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    ESP_LOGI(TAG, "GPIO-Pins konfiguriert.");
}

void lcd_init_display(void)
{
    ESP_LOGI(TAG, "Initialisiere LCD über GPIO...");

    lcd_gpio_init();
    lcd_reset_display();

    lcd_send_command(0x11); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(120));

    lcd_send_command(0x3A); // Pixel Format
    lcd_send_data(0x55);    // 16 Bit

    lcd_send_command(0x36); // Memory Access Control
    lcd_send_data(0x48);    // MX, BGR

    lcd_send_command(0x29); // Display ON
    vTaskDelay(pdMS_TO_TICKS(20));

    ESP_LOGI(TAG, "LCD-Initialisierung abgeschlossen.");
}
