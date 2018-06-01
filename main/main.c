#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "app_wifi.h"
#include "repl.h"

static const char *TAG = "HTTP_TEST";


void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
	  ESP_ERROR_CHECK(nvs_flash_erase());
	  ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	app_wifi_initialise();

//    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
//    int level = 0;
//    while (true) {
//        gpio_set_level(GPIO_NUM_4, level);
//        level = !level;
//        vTaskDelay(300 / portTICK_PERIOD_MS);
//    }

	app_wifi_wait_connected();
	ESP_LOGI(TAG, "Connected to AP, begin REPL");

    xTaskCreate(repl_task, "repl_task", 8192, NULL, 5, NULL);
}

