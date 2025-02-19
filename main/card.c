#include "card.h"
#include "define.h"
#include "mqtt.h"
#include <string.h>
#include <sys/time.h>

#include "esp_sntp.h"
#include "esp_log.h"

static const char *TAG = "SNTP";

epc_card_t  head;
static void card_list_init()
{
    memset(&head,0x00,sizeof(epc_card_t));
    head.next = NULL;
}


static char* card_get_userdata(uint8_t *data,int len)
{
    uint8_t *p = data + 4;

    char *tmp = (char *)malloc(len * 2 + 1);
    for (int i = 0; i < len; i++) {
        sprintf(tmp + i * 2, "%02X", p[i]);  
    }
    return tmp;
}

static epc_card_t *find_card(char *data,int len)
{
    if (head.user_data == NULL) {
        head.user_data = data;
        head.timestamp = 0;
        head.next = NULL;
        return &head;
    }

    epc_card_t *p = &head;
    while (p != NULL) {
        if (memcmp(data, p->user_data, len) == 0) {
            return p;
        }
        if (p->next == NULL) {
            break;
        }
        p = p->next;
    }

    epc_card_t *new_node = malloc(sizeof(epc_card_t));
    if (new_node == NULL) {
        printf("Memory allocation failed!\n");
        return NULL; 
    }
    new_node->user_data = data;
    new_node->next = NULL;
    new_node->timestamp = 0;
    p->next = new_node;
    return new_node;
}
static void card_send_userdata(char *data)
{
    app_mqtt_send_task(data);
}

static void card_get_and_send_userdata(uint8_t *data,int len)
{
    epc_card_t *cur_card;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t current_timestamp_us = (uint64_t)tv.tv_sec * 1000000L + tv.tv_usec;
    len -= 6;

    char *tmp = card_get_userdata(data,len);
    cur_card = find_card(tmp,len);
    if (cur_card->timestamp == 0) {
        cur_card->timestamp = current_timestamp_us;
        printf("new card\n");
    }else{
        uint64_t time_diff = current_timestamp_us - cur_card->timestamp;
        cur_card->timestamp = current_timestamp_us;
        printf("time_diff: %llu\n", time_diff);
    }

    card_send_userdata(tmp);
}

static void read_card_task(void *arg)
{
    uint8_t uart_buf[128];
    
    while(1)
    {
        memset(uart_buf,0x00,sizeof(uart_buf));
       
        int len = uart_read_bytes(UART_NUM, uart_buf, 8, portMAX_DELAY);
        if (len > 0)
            card_get_and_send_userdata(uart_buf,len);
            
        
    }
}
void card_init()
{
    card_list_init();
     // 初始化 SNTP
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org"); // 使用 NTP 服务器
    esp_sntp_set_time_sync_notification_cb(NULL);
    esp_sntp_init();

    // 等待时间同步
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (retry == retry_count) {
        ESP_LOGE(TAG, "Failed to sync time with SNTP server!");
        return;
    }
    xTaskCreate(read_card_task,"read_card",16384,NULL,15,NULL);
}
