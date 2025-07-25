#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"


#define WIFI_SSID "myssid"
#define WIFI_PASS "mypassword"
#define MQTT_BROKER_URI "mqtt://broker.emqx.io"

static const char *TAG = "sensor";
static esp_mqtt_client_handle_t mqtt_client = NULL;

void wifi_init(void){

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void mqtt_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    esp_mqtt_event_t *event = (esp_mqtt_event_t *)event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
    }
}

void mqtt_init(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URI,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "Failed to create MQTT client");
        return;
    }
    esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void dummy sensor (void *pvParameters){
    while (1){
        int temp = 20+ esp_random() % 10; // Simulate temperature reading
        int humidity = 50 + esp_random() % 20; // Simulate humidity reading

        char msg[100];
        snprintf(msg, sizeof(msg), "{\"temperature\": %d, \"humidity\": %d}",
                    temp, humidity);
                    ESP_LOGI(TAG, "Publishing: %s", msg);

        //USE TOPIC DEPENDING UPON BROKER 
        esp_mqtt_client_publish(mqtt_client, "sensor/data", msg, 0, 1, 0);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Publish every 5 seconds
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting application...");

    wifi_init();
    mqtt_init();

    xTaskCreate(dummy_sensor, "dummy_sensor", 4096, NULL, 5, NULL);
}