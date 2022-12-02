// SDK: https://github.com/espressif/ESP8266_RTOS_SDK/
// API reference:
// https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-reference
// Docs: https://freertos.org/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static const char* TAG = "wifi";

#include "./include/blink.c"
#include "./include/wifi.c"

// #include "./include/mqtt.c"

void app_main() {
	//   setup_mqtt();
	setup_wifi();
	vTaskDelay(5000 / portTICK_RATE_MS);
	setup_blink();
}