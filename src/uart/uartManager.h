#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <Arduino.h>
#include "data/dataHolder.h"
#include "freertos/semphr.h"
#include "driver/timer.h"


#define EX_UART_NUM UART_NUM_2
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

#define TIMER_DIVIDER   (80)


struct ReceivedData {
    uint8_t key;
    uint8_t value;
    bool gotKey;
};


void reverseString(char *str);

class UartManager {

    public:
        typedef void (*DataChangedCallback)(uint8_t);

        static QueueHandle_t uart0_queue;
        static QueueHandle_t dataChangedQueue;
        static DataHolder* dataHolder;
        static SemaphoreHandle_t timerSem;

        void sendData(uint8_t key , uint8_t value);
        static bool IRAM_ATTR timerCallback(void* arg);
        static void notifyDataChanged(uint8_t dataKey);
        static void parseReceivedData(ReceivedData* receivedData, uint8_t* data , char* powerConsumptionBuffer);

        void setupUartFactory(DataHolder* dataHolder , DataChangedCallback* dataChangedCallback);

    private:
        static void onDataChangedListner(DataChangedCallback* dataChangedCallback);
        void initUart(DataHolder* dataHolder);
        void registerTimerToGetPowerConsumptionAndTemp();
        void notifiyMicroControllerToGetPowerConsump();
        void registerDataChangedCallback(DataChangedCallback* callback);
};