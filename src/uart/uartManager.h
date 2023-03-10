#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <Arduino.h>
#include "data/dataHolder.h"


#define EX_UART_NUM UART_NUM_2
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

struct ReceivedData {
    uint8_t key;
    uint8_t value;
    bool gotKey;
};

class UartManager {

    public:
        typedef void (*DataChangedCallback)(DataItem);

        static QueueHandle_t uart0_queue;
        static QueueHandle_t dataChangedQueue;

        void initUart();
        void sendData(DataItem dataItem);
        static void registerDataChangedCallback(DataChangedCallback* callback);
        static void notifyDataChanged(DataItem dataItem);

    private:
        static void onDataChangedListner(DataChangedCallback* dataChangedCallback);
};