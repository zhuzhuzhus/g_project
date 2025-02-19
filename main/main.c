#include  <define.h>
#include <mqtt.h>
#include <uart.h>
#include <card.h>


void _uart_debug()
{
    uint8_t uart_buf[128]; 
    while (1)
    {

        int len = uart_read_bytes(UART_NUM, uart_buf, 8, 100);
        if (len > 0)
        {
            printf("Received %d bytes: ", len);

           
            for (int i = 0; i < len; i++)
            {
                printf("%02X ", uart_buf[i]);  
            }
            printf("\n");
        }

    }
}
void app_main(void)
{
    printf("uart");
    uart_init();
    // _uart_debug();
    printf("wifi");
    wifi_start();
    vTaskDelay(100);
    mqtt_app_start();
    card_init();
}

