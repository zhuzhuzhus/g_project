#ifndef __ESP_MQTT_H__
#define __ESP_MQTT_H__
void mqtt_app_start(void);
void wifi_start(void);
void app_mqtt_send_task(char *card_id);

#endif