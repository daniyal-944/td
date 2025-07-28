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
#include "esp_system.h"
#include "esp_random.h"


#define WIFI_SSID "myssid"
#define WIFI_PASS "mypassword"
#define MQTT_BROKER_URI "mqtt://mqtt.thingsboard.cloud:1883"

static const char *TAG = "sensor";
static esp_mqtt_client_handle_t mqtt_client = NULL;

void wifi_init(void){
    static const char *TAG_WIFI = "wifi_init";
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_LOGI(TAG_WIFI, "Setting WiFi configuration SSID %s...", WIFI_SSID);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Connect manually
    ESP_ERROR_CHECK(esp_wifi_connect());
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
    
        case MQTT_EVENT_SUBSCRIBED:
        case MQTT_EVENT_UNSUBSCRIBED:
        case MQTT_EVENT_PUBLISHED:
        case MQTT_EVENT_DATA:
        case MQTT_EVENT_ERROR:
        case MQTT_EVENT_BEFORE_CONNECT:
        case MQTT_EVENT_DELETED:
        case MQTT_USER_EVENT:
        break;
    
    default:
        ESP_LOGW(TAG, "Unhandled MQTT event: %d", event->event_id);
        break;
    }
}


void mqtt_init(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .credentials.username = "VmPHEK7CVlEvK7L8b38W", // Replace with your MQTT username
    };

    esp_mqtt_client_handler_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "Failed to create MQTT client");
        return;
    }
    esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void dummy_sensor (void *pvParameters){
    while (1){
        int temp = 20+ esp_random() % 10; // Simulate temperature reading
        int humidity = 50 + esp_random() % 20; // Simulate humidity reading

        char msg[100];
        snprintf(msg, sizeof(msg), "{\"temperature\": %d, \"humidity\": %d}",
                    temp, humidity);
                    ESP_LOGI(TAG, "Publishing: %s", msg);

        //USE TOPIC DEPENDING UPON BROKER 
        esp_mqtt_client_publish(mqtt_client, "v1/devices/me/telemetry", msg, 0, 1, 0);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Publish every 5 seconds
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting application...");

    wifi_init();

    // Wait some time for WiFi to connect (temporary workaround)
    vTaskDelay(pdMS_TO_TICKS(5000));

    mqtt_init();

    xTaskCreate(dummy_sensor, "dummy_sensor", 4096, NULL, 5, NULL);
}
