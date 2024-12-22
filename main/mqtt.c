#include <mqtt.h>
#include <define.h>
#include "mqtt_client.h"
#define TAG "MQTT"
#define   Aliyun_hostname   "iot-06z00ef47mzwg1p.mqtt.iothub.aliyuncs.com" //或称mqttHostUrl、Broker Address
#define   Aliyun_port       1883
#define   Aliyun_client_id  "k24s9YGmoas.UHF_RFID|securemode=2,signmethod=hmacsha256,timestamp=1734866480105|"
#define   Aliyun_username   "UHF_RFID&k24s9YGmoas"
#define   Aliyun_password   "e46ba6d58665a38e84d85509ed1163865de671a15b04ab01be922f7f435ce389"
#define   Aliyun_testtopic  "/sys/k24s9YGmoas/UHF_RFID/thing/event/property/post"

#define wifi_name           "407"
#define wifi_password       "zhujiale"

esp_mqtt_client_handle_t client;

bool get_ip = false;

static void mqtt_event_handler(void* handler_arg,esp_event_base_t event_base,int32_t event_id,void* event_data)
{

    esp_mqtt_event_handle_t event = event_data;
    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            if (esp_mqtt_client_subscribe(client, "/topic/ip_mesh/key_pressed", 0) < 0) {
                // Disconnect to retry the subscribe after auto-reconnect timeout
                esp_mqtt_client_disconnect(client);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }

}

static void app_mqtt_send_task(void *arg)
{
    // const char *str = malloc(100 + sizeof("{\"method\":\"thing.service.property.set\",\"id\":\"1101\",\"params\":{\"rfidid\":\"zhujialetest\"},\"version\":\"1.0.0\"}"));
    char str[256];
    int i=0;
    while(1)
    {
        i++;
        if(i == 100000)
            i=0;
        // snprintf(str,sizeof(str),"{\"method\":\"thing.service.property.set\",\"id\":\"%d\",\"params\":{\"rfidid\":\"zhujialetest\"},\"version\":\"1.0.0\"}",i);
        snprintf(str,sizeof(str),"{\"method\":\"thing.service.property.set\",\"id\":\"11111\",\"params\":{\"rfidid\":\"%d\"},\"version\":\"1.0.0\"}",i);
        esp_mqtt_client_publish(client,Aliyun_testtopic,str,strlen(str),0,0);
        vTaskDelay(500);
    }

}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        // idf 新版本(esp-idf-V5.2.1)参数配置如下
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
        .broker.address.hostname = Aliyun_hostname,
        .broker.address.port = Aliyun_port,
        .credentials.client_id = Aliyun_client_id,
        .credentials.username = Aliyun_username,
        .credentials.authentication.password = Aliyun_password,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    xTaskCreate(app_mqtt_send_task,"mqtt",2048,NULL,15,NULL);
}


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch(event_id)
        {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                printf("wifi connect...\n");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                printf("connect!\n");
                break;
            default:
            break;
        }
    }
    if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        printf("get ip : %d.%d.%d.%d \n",IP2STR(&event->ip_info.ip));
        get_ip = true;
    }
}

void wifi_start()
{
    nvs_flash_init();
    esp_netif_init();

    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg) ;

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL); 
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = wifi_name,
            .password = wifi_password,
        },
    };

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    while(get_ip == false) 
    {
        vTaskDelay(100);
        printf(".");
    }
     printf("\n");
}