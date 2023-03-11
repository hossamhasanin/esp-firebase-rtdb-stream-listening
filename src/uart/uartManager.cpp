#include "uartManager.h"

QueueHandle_t UartManager::uart0_queue;
QueueHandle_t UartManager::dataChangedQueue;
DataHolder* UartManager::dataHolder = nullptr;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    ReceivedData data;
    char powerConsumptionBuffer[4];
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
                    UartManager::parseReceivedData(&data, dtmp , powerConsumptionBuffer);
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

void UartManager::parseReceivedData(ReceivedData* receivedData, uint8_t* data , char* powerConsumptionBuffer){
    if (!receivedData->gotKey){
        receivedData->key = atoi((char*)data);
        if (!DataHolder::isKeyValid(receivedData->key)){
            Serial.println("uart key is not valid");
            return;
        }
        receivedData->gotKey = true;
    } else {
        if (receivedData->key == powerConsumptionId){
            // check if character (*) is received
            if (*data == '*'){
                receivedData->gotKey = false;
                // reverse the powerConsumptionBuffer
                reverseString(powerConsumptionBuffer);
                double powerConsumption = atof(powerConsumptionBuffer)/1000;
                Serial.print("powerConsumption done: ");
                Serial.println(powerConsumption);
                dataHolder->setPowerConsumption(powerConsumption);
                UartManager::notifyDataChanged(receivedData->key);
                bzero(powerConsumptionBuffer, 4);
                return;
            }
            // concatinate (char* data) in recieving the power consumption number
            strcat(powerConsumptionBuffer, (char*)data);
            Serial.print("powerConsumptionBuffer: ");
            Serial.println(powerConsumptionBuffer);
        } else {
            receivedData->value = atoi((char*)data);
            receivedData->gotKey = false;
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
            UartManager::notifyDataChanged(receivedData->key);
        }
    }
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
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
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