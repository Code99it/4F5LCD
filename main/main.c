/**
 * main.c
 *
 * Copyright (C) 05/2025 Christian Wilhelm, cw1@code99.it
 * 
 * Permission is granted to use this code for any private purpose;
 * commercial use or sale of this code is strictly prohibited.
 * 
 * Es ist gestattet, diesen Code privat beliebig zu verwenden oder zu ändern;
 * kommerzielle Nutzung oder Verkauf dieses Codes und damit verbundener 
 * Komplettlösungen oder Produkte ist strengstens untersagt.
 * 
 * **/
 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "4F5LCD";

void app_main(void)
{
    ESP_LOGI(TAG, "READY.");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
