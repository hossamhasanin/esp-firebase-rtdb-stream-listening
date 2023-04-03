#include "uartManager.h"

QueueHandle_t UartManager::uart0_queue;
QueueHandle_t UartManager::dataChangedQueue;
DataHolder* UartManager::dataHolder = nullptr;
SemaphoreHandle_t UartManager::timerSem = NULL;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    ReceivedData data;
    char receivedNumbersBuffer[16];
    for(;;) {
        // Serial.println("Uart running");
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
                    Serial.printf("uart[%d] read: %s\n", EX_UART_NUM, (char*)dtmp);
                    UartManager::parseReceivedData(&data, dtmp , receivedNumbersBuffer);
                    // cast dtmp int print dtmp to console using printf
                    // uart_write_bytes(EX_UART_NUM, (const char*) dtmp, event.size);
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

void reverseString(char *str) {
    int len = strlen(str);
    int i, j;
    char temp;

    for (i = 0, j = len - 1; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

void UartManager::parseReceivedData(ReceivedData* receivedData, uint8_t* data , char* receivedNumbersBuffer){

    while(*data != 0){
        Serial.printf("UART receivedNumbersBuffer so far: %s \n", receivedNumbersBuffer);

        if (*data == ',' || *data == '/'){

            if (*data == ','){
                receivedData->key = atoi(receivedNumbersBuffer);
                Serial.printf("UART got a key: %d \n", receivedData->key);
                receivedData->gotKey = true;
                // clear the receivedNumbersBuffer
                memset(receivedNumbersBuffer, 0, sizeof(receivedNumbersBuffer));
            } else {
                receivedData->gotKey = false;
                if (receivedData->key == powerConsumptionId){
                    double powerConsumption = atof((char *)receivedNumbersBuffer)/1000;
                    Serial.print("powerConsumption recieved: ");
                    Serial.println(powerConsumption);
                    dataHolder->setPowerConsumption(powerConsumption);
                } else {
                    receivedData->value = atoi(receivedNumbersBuffer);
                    if (receivedData->key == tempId){
                        dataHolder->setTemp(receivedData->value);
                    } else if (receivedData->key == led1Id){
                        dataHolder->setLed1(receivedData->value);
                    } else if (receivedData->key == electriId){
                        dataHolder->setElectri(receivedData->value);
                    } else if (receivedData->key == passwordWrongAlarmId){
                        dataHolder->setPasswordWrongAlarm(receivedData->value);
                    } else if (receivedData->key == numOfPeopleId){
                        dataHolder->setNumOfPeople(receivedData->value);
                    } else if (receivedData->key == gasLeakAlarmId) {
                        dataHolder->setGasLeakAlarm(receivedData->value);
                    } else if (receivedData->key == rgblStateId) {
                        dataHolder->setRgblState(receivedData->value);
                    } else if (receivedData->key == doorStateId) {
                        dataHolder->setDoorState(receivedData->value);
                    }
                }
                UartManager::notifyDataChanged(receivedData->key);
                const char star = RECIEVED_VALUE_FLAG;
                uart_write_bytes(EX_UART_NUM, &star, 1);
                memset(receivedNumbersBuffer, 0, sizeof(receivedNumbersBuffer));
            }

        } else {
            char num_str[4];
            sprintf(num_str, "%c", *data);
            strcat(receivedNumbersBuffer, num_str);
            // print data
            Serial.printf("UART got a data: %c \n", *data);
            Serial.printf("UART receivedNumbersBuffer so far: %s \n", receivedNumbersBuffer);
        }

        data++;
    }
}

void UartManager::setupUartFactory(DataHolder* dataHolder , DataChangedCallback* dataChangedCallback){
    this->initUart(dataHolder);
    this->registerDataChangedCallback(dataChangedCallback);
    // this->registerTimerToGetPowerConsumptionAndTemp();
    // this->notifiyMicroControllerToGetPowerConsump();
}

void UartManager::initUart(DataHolder* dataHolder){
    UartManager::dataHolder = dataHolder;

    dataChangedQueue = xQueueCreate(10, sizeof(uint8_t));

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
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 1, NULL);
}


void UartManager::notifyDataChanged(uint8_t dataKey){
    if (xQueueSend(dataChangedQueue, (void *) &dataKey, (TickType_t) 2) != pdPASS){
        Serial.println((const char *)FPSTR("stream queue full"));
      }
}

void UartManager::onDataChangedListner(DataChangedCallback* dataChangedCallback){
    uint8_t dataKey;
    if (xQueueReceive(dataChangedQueue, &dataKey, (TickType_t) 2) == pdPASS){
        // if (dataChanged.key != NULL) {
        // Do something here
        Serial.printf((const char *)FPSTR("Some data has been changed"));
        Serial.println(dataKey);
        (*dataChangedCallback)(dataKey);
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

void UartManager::sendData(uint8_t key , uint8_t value){
    char key_str[5]; // assuming the key is a signed 8-bit integer
    char value_str[4]; // assuming the value is an unsigned 8-bit integer

    // convert key and value to strings using sprintf
    sprintf(key_str, "%d", key);
    sprintf(value_str, "%u", value);
    uart_write_bytes(EX_UART_NUM, key_str, strlen(key_str));
    uart_write_bytes(EX_UART_NUM, value_str, strlen(value_str));
}

bool IRAM_ATTR UartManager::timerCallback(void* arg){
    Serial.println((const char *)FPSTR("Get power consumption from uart interrupt"));

    BaseType_t high_task_awoken = pdFALSE;
    xSemaphoreGiveFromISR(timerSem, &high_task_awoken);
    return (high_task_awoken == pdTRUE);
}

void UartManager::registerTimerToGetPowerConsumptionAndTemp(){
    timerSem = xSemaphoreCreateBinary();
    if (timerSem == NULL) {
        printf("Binary semaphore can not be created");
    }
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = TIMER_DIVIDER
    };
    timer_init(TIMER_GROUP_0, TIMER_1, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_1, 0);
    // set alarm to 30 seconds
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, 30000000);

    timer_enable_intr(TIMER_GROUP_0, TIMER_1);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_1, UartManager::timerCallback, NULL, 0);
    timer_start(TIMER_GROUP_0, TIMER_1);
}

void UartManager::notifiyMicroControllerToGetPowerConsump(){
    if (timerSem != NULL){
        // create a task to get power consumption
        xTaskCreate([](void *param) {
            while (true) {
                if (xSemaphoreTake(timerSem, portMAX_DELAY) == pdPASS) {
                    Serial.println((const char *)FPSTR("Sending get command to mc"));
                    char key_str[2]; // assuming the key is a signed 8-bit integer
                    // convert key and value to strings using sprintf
                    sprintf(key_str, "%d", tempId);
                    uart_write_bytes(EX_UART_NUM, key_str, strlen(key_str));
                    Serial.println((const char *)FPSTR("Sent get command to mc"));
                }
            }
        }, "getPowerConsumptionTask", 2048, NULL, 6, NULL);
    }
}