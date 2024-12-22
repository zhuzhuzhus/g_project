#include  <define.h>
#include <mqtt.h>
static uint8_t uart_buffer[1024];
static QueueHandle_t uart_queue;
void uart_init()
{    uart_config_t uart0_cfg = 
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .parity = UART_PARITY_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
        .stop_bits =  UART_STOP_BITS_1,
    };
    uart_param_config(UART_NUM_0, &uart0_cfg);
    uart_driver_install(UART_NUM_0,1024,1024,20,NULL,0);

    uart_config_t uart_cfg = 
    {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .parity = UART_PARITY_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
        .stop_bits =  UART_STOP_BITS_1,
    };

    uart_param_config(UART_NUM, &uart_cfg);
    uart_set_pin(UART_NUM,GPIO_NUM_17,GPIO_NUM_18,-1,-1);
    uart_driver_install(UART_NUM,1024,1024,20,&uart_queue,0);
}

void app_main(void)
{
    printf("uart");
    uart_init();
    printf("wifi");
    wifi_start();
    vTaskDelay(100);
    mqtt_app_start();
}

