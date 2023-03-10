#include "uartManager.h"

QueueHandle_t UartManager::uart0_queue;
QueueHandle_t UartManager::dataChangedQueue;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    ReceivedData data;
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(UartManager::uart0_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            Serial.printf("uart[%d] event:\n", EX_UART_NUM);
            switch(event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
                    Serial.printf("data, len: %d;\n", event.size);
                    uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    if (!data.gotKey){
                        data.key = atoi((char*)dtmp);
                        data.gotKey = true;
                    } else {
                        data.value = atoi((char*)dtmp);
                        data.gotKey = false;
                        DataItem dataItem;
                        dataItem.key = data.key;
                        dataItem.value = data.value;
                        UartManager::notifyDataChanged(dataItem);
                    }
                    // cast dtmp int print dtmp to console using printf
                    Serial.printf("uart[%d] read: %s\n", EX_UART_NUM, (char*)dtmp);
                    uart_write_bytes(EX_UART_NUM, (const char*) dtmp, event.size);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    Serial.println("hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(UartManager::uart0_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    Serial.println("ring buffer full");
                    // If buffer full happened, you should consider increasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(UartManager::uart0_queue);
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    Serial.println("uart rx break");
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    Serial.println("uart parity error");
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    Serial.println("uart frame error");
                    break;
                //Others
                default:
                    Serial.printf("uart event type: %d\n", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void UartManager::initUart(){

    dataChangedQueue = xQueueCreate(10, sizeof(DataItem));

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    //Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);

    //Set UART pins (using UART2 default pins ie no changes.)
    uart_set_pin(EX_UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
}


void UartManager::notifyDataChanged(DataItem dataItem){
    if (xQueueSend(dataChangedQueue, (void *) &dataItem, (TickType_t) 2) != pdPASS){
        Serial.println((const char *)FPSTR("stream queue full"));
      }
}

void UartManager::onDataChangedListner(DataChangedCallback* dataChangedCallback){
    DataItem dataChanged;
    if (xQueueReceive(dataChangedQueue, &dataChanged, (TickType_t) 2) == pdPASS){
        // if (dataChanged.key != NULL) {
        // Do something here
        Serial.println((const char *)FPSTR("Some data has been changed"));
        Serial.println(dataChanged.key);
        Serial.println(dataChanged.value);
        (*dataChangedCallback)(dataChanged);
        // }
    }
}

void UartManager::registerDataChangedCallback(DataChangedCallback* dataChangedCallback){
    xTaskCreate([](void *param) {
      while (true) {
        DataChangedCallback* c = (DataChangedCallback*) param;
        if (dataChangedQueue != NULL) {
          UartManager::onDataChangedListner(c);
        }
      }
    }, "dataChangedCallbackTask", 6048, dataChangedCallback, 12, NULL);
}

void UartManager::sendData(DataItem dataItem){
    char key_str[5]; // assuming the key is a signed 8-bit integer
    char value_str[4]; // assuming the value is an unsigned 8-bit integer

    // convert key and value to strings using sprintf
    sprintf(key_str, "%d", dataItem.key);
    sprintf(value_str, "%u", dataItem.value);
    uart_write_bytes(EX_UART_NUM, key_str, strlen(key_str));
    uart_write_bytes(EX_UART_NUM, value_str, strlen(value_str));
}