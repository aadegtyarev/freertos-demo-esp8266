// Пример с gpio:
// https://github.com/espressif/ESP8266_RTOS_SDK/blob/d45071563cebe9ca520cbed2537dc840b4d6a1e6/examples/peripherals/gpio/main/user_main.c

#include "driver/gpio.h"

// Описываем пины, которые будем использовать
#define GPIO_OUTPUT_IO_0 15
#define GPIO_OUTPUT_IO_1 16
#define GPIO_OUTPUT_PIN_SEL \
	((1ULL << GPIO_OUTPUT_IO_0) | (1ULL << GPIO_OUTPUT_IO_1))

// Мигаем светодиодом GPIO_OUTPUT_IO_0
void blink1(void *pvParameters) {
	while (1) {
		gpio_set_level(GPIO_OUTPUT_IO_0, 0);
		vTaskDelay(1000 / portTICK_RATE_MS);
		gpio_set_level(GPIO_OUTPUT_IO_0, 1);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

// Мигаем светодиодом GPIO_OUTPUT_IO_1
void blink2(void *pvParameters) {
	while (1) {
		gpio_set_level(GPIO_OUTPUT_IO_1, 0);
		vTaskDelay(250 / portTICK_RATE_MS);
		gpio_set_level(GPIO_OUTPUT_IO_1, 1);
		vTaskDelay(250 / portTICK_RATE_MS);
	}
}

void setup_blink() {
	// Задаём настройки для GPIO
	gpio_config_t io_conf;
	// Отключаем прерывания
	io_conf.intr_type = GPIO_INTR_DISABLE;
	// Переключаем GPIO в режим выхода
	io_conf.mode = GPIO_MODE_OUTPUT;
	// Битовая маска GPIO, которые будут выходами
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	// Отключаем подтяжки к минусу и плюсу (pull-down and pull-up modes)
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 0;

	// Конфигурируем GPIO описанными выше настройками
	gpio_config(&io_conf);

	// Создаём задачи
	xTaskCreate(blink1, "blink1", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(blink2, "blink2", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
}