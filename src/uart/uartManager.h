#pragma once

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <Arduino.h>
#include "freertos/semphr.h"
#include "driver/timer.h"
#include "data/devices/updateDevicesCallback.h"
#include "data/devices/devicesManager.h"


#define EX_UART_NUM UART_NUM_2
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

#define TIMER_DIVIDER   (80)

#define RECIEVED_VALUE_FLAG '*'


struct ReceivedData {
    uint8_t key;
    uint8_t value;
    bool gotKey;
};


void reverseString(char *str);

class UartManager : public UpdateDevicesCallback{

    public:
        typedef void (*DataChangedCallback)(uint8_t);

        static QueueHandle_t uart0_queue;
        static QueueHandle_t dataChangedQueue;
        static DevicesManager* dataHolder;
        static SemaphoreHandle_t timerSem;
        static bool* stopSendingDataMutex;

        static void sendData(uint8_t key , uint8_t value);
        static bool IRAM_ATTR timerCallback(void* arg);
        static void notifyDataChanged(uint8_t dataKey);
        static void parseReceivedData(ReceivedData* receivedData, uint8_t* data , char* powerConsumptionBuffer);


        void setupUartFactory(DevicesManager* dataHolder , DataChangedCallback* dataChangedCallback , bool* stopSendingDataMutex);

        void updateSwitch(Switch *device);
        void updateTempratureSensor(TempratureSensor *device);
        void updatePowerConsumption(PowerConsumption *device);
        void updatePeopleCounter(PeopleCounter *device);
        void updateRgbLight(RgbLight *device);
        void updateAc(AcCommands *device);

    private:
        static void onDataChangedListner(DataChangedCallback* dataChangedCallback);
        void initUart(DevicesManager* dataHolder);
        void registerTimerToGetPowerConsumptionAndTemp();
        void notifiyMicroControllerToGetPowerConsump();
        void registerDataChangedCallback(DataChangedCallback* callback);
};