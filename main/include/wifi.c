#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

// Запустите в консоли $ make menuconfig , чтобы изменить настройки подключения

#define DEMO_WIFI_SSID CONFIG_WIFI_SSID
#define DEMO_WIFI_PASSWORD CONFIG_WIFI_PASSWORD
#define DEMO_HOSTNAME CONFIG_HOSTNAME

// Объявляем группу событий
static EventGroupHandle_t wifi_event_group;

/* В группе событий можно использовать несколько битов для каждого события,
но нас волнует только одно событие — «Подключены ли мы к AP и получили ли IP?»
*/
const int WIFI_CONNECTED_BIT = BIT0;

// Обработка событий
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
				   void *event_data) {
	// Если Wi-Fi клиент запущен, то
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		// подключаемся
		esp_wifi_connect();

		// Если клиент отключен
	} else if (event_base == WIFI_EVENT &&
			   event_id == WIFI_EVENT_STA_DISCONNECTED) {
		system_event_sta_disconnected_t *event =
			(system_event_sta_disconnected_t *)event_data;

		// и выбранный нами режим не поддерживается точкой доступа, то
		if (event->reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
			/// меняем режим Wi-Fi на 802.11 bgn
			esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B |
													   WIFI_PROTOCOL_11G |
													   WIFI_PROTOCOL_11N);
		}

		// и подключаемся
		esp_wifi_connect();

		// Также сбрасываем WIFI_CONNECTED_BIT в группе событий wifi_event_group
		xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);

		// Если IP нами получен, то
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		// устанавливаем WIFI_CONNECTED_BIT в группе wifi_event_group
		xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

void initialise_wifi(void) {
	// Задаём имя хоста, которое будет видно в роутере при подключении по Wi-Fi
	tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, DEMO_HOSTNAME);

	// Инициализируем tcpip адаптер
	tcpip_adapter_init();

	// Создаём группу событий RTOS wifi_event_group и запускаем прослушку
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Загружаем в cfg конфигурацию по умолчанию и инициализируем Wi-Fi
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// Подписываемся на события Wi-Fi и TCP
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
											   &event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
											   &event_handler, NULL));

	// Указываем, что хранить конфигурацию Wi-Fi будем во флеш
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	// Указываем настройки подключения к точке доступа
	wifi_config_t wifi_config = {
		.sta =
			{
				.ssid = DEMO_WIFI_SSID,
				.password = DEMO_WIFI_PASSWORD,
			},
	};

	// Выводим в лог SSID точки доступа к которой будем подключатся.
	ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...",
			 wifi_config.sta.ssid);
	// Устанавливаем режим клиента
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	// Передаём указанные выше настройки подключения компоненту Wi-Fi
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	// Запускаем Wi-Fi
	ESP_ERROR_CHECK(esp_wifi_start());
}

void setup_wifi() {
	// Инициализируем NVS
	ESP_ERROR_CHECK(nvs_flash_init());

	// Инициализируем Wi-Fi
	initialise_wifi();
}
