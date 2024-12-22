#include  "define.h"
#include "rfid.h"
#include "mqtt.h"
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
    // uart_set_pin(UART_NUM_0,GPIO_NUM_17,GPIO_NUM_18,-1,-1);
    uart_driver_install(UART_NUM_0,1024,1024,20,NULL,0);
            

    uart_event_t uart_ev;
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
    uint8_t IC[50] = {0};
    uart_init();
    
    rfid_init();
    while(1)
    {
        uint8_t r_value;
        uart_read_bytes(UART_NUM, &r_value, sizeof(uint8_t), 1000 / portTICK_PERIOD_MS);
        printf("0x%x \n",r_value);
        ets_delay_us(500*1000);
        rfid_uart_write(BitFramingReg,0x07);
    }

    while(1){
		if(RC522_READ(IC)){
			printf("%s\r\n",IC);
			ets_delay_us(500*1000);
		}
		ets_delay_us(1000*1000);
		printf("waitting...\r\n");
	} 
}

    // while(1)
    // {
    //     if(pdTRUE ==xQueueReceive(uart_queue,&uart_ev,portMAX_DELAY))
    //     {
    //         switch (uart_ev.type)
    //         {
    //         case UART_DATA:
    //             ESP_LOGI(TAG,"UART receive len :%i",uart_ev.size);
    //             uart_read_bytes(UART_NUM,uart_buffer,uart_ev.size,pdMS_TO_TICKS(100));
    //             uart_write_bytes(UART_NUM,uart_buffer,uart_ev.size);
    //             break;
            
    //         case UART_BUFFER_FULL:
    //             uart_flush_input(UART_NUM);
    //             xQueueReset(uart_queue);
    //             break;
    //         case UART_FIFO_OVF:
    //             uart_flush_input(UART_NUM);
    //             xQueueReset(uart_queue);
    //             break;
    //         default:
    //             break;
    //         }
    //     }
    //     // int ret = uart_read_bytes(UART_NUM,uart_buffer,1024,pdMS_TO_TICKS(50));
    //     // if(ret)
    //     // {
    //     //     uart_write_bytes(UART_NUM,uart_buffer,ret);
    //     // }


    // }
