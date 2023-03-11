#pragma once

#include <Firebase_ESP_Client.h>
#include "freertos/FreeRTOS.h"
#include "data/dataItem.h"
#include "data/dataHolder.h"
#include "freertos/semphr.h"
#include "driver/timer.h"

/* 2. Define the API Key */
#define API_KEY "API_KEY"

/* 3. Define the RTDB URL */
#define DATABASE_URL "voiced-home-assistant-default-rtdb.europe-west1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define DATA_PATH "/states"

#define POWER_CONSUMPTION_PATH "/powerConsumption"

#define LAST_ONLINE_PATH "/lastOnlineTime"

#define TIMER_DIVIDER   (80)




class FirebaseListener{

    
    public:
        // typedef DataItem (*DataParsingCallback)(int , uint8_t);
        typedef void (*DataChangedCallback)(uint8_t);

        static SemaphoreHandle_t timerSem;
        static FirebaseData fbdo;
        static DataHolder* data;

    private:
        FirebaseConfig config;
        FirebaseAuth auth;
        TaskHandle_t _dataChangeHandle;
        static TaskHandle_t _timerHandle;

        static bool IRAM_ATTR timerCallback(void* arg);

        // static DataParsingCallback dataParsingCallback;
        static FirebaseData stream;
        // create queue to detect if data is changed
        static QueueHandle_t queueFlagChangedData;
        static void streamCallback(FirebaseStream data);
        static void streamTimeoutCallback(bool timeout);
        static void onDataChangedEvent(DataChangedCallback* callback);
        static void notifyDataChangedToQueue(uint8_t dataKey);

    public:
        void init(DataHolder* data);
        void start(int maxQueueSize);
        void stop();
        void registerDataChangeTask(DataChangedCallback* callback);
        void registerTimerToUpdateLastOnline();

        void storeInt(const char* key, int value);
        void storeDouble(const char* key, double value);
        void storeBool(const char* key, bool value);
        void storePowerConsumption(double value);
        void updateTimaStamp();
        
        // static void setDataParsingCallback(DataParsingCallback callback);
    
};