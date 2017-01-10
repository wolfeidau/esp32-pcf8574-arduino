#include "wifi.h"
#include "ioextender.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

static const char *TAG = "esp32-pcf8574";

static void send_buttons_task(void *pvParameter);

extern "C" void app_main()
{
    //Serial.begin(115200);
    
    wifi_initialize();
    ioextender_initialize();

    xTaskCreatePinnedToCore(send_buttons_task, "send_buttons_task", 4096, NULL, 1, NULL, 1);
}

static void send_buttons_task(void *pvParameter) {

    QueueHandle_t buttons = xQueueCreate(10, sizeof(button_reading_t));

    if (! buttons_subscribe(buttons)) {
        ESP_LOGE(TAG, "Failed to subscribe to button readings :(");
    }

    while (1) {
        button_reading_t reading;
        if (xQueueReceive(buttons, &reading, 6000 / portTICK_PERIOD_MS)) {
            ESP_LOGI(TAG, "%s state %s",
                    reading.label,
                    reading.state);
            // mqtt_publish_sensor(reading.label, reading.state)
        }
    }
}