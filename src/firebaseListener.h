#include <Firebase_ESP_Client.h>
#include "freertos/FreeRTOS.h"
#include "data/dataItem.h"

/* 2. Define the API Key */
#define API_KEY "API_KEY"

/* 3. Define the RTDB URL */
#define DATABASE_URL "voiced-home-assistant-default-rtdb.europe-west1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define DATA_PATH "/states"





class FirebaseListener{

    
    public:
        typedef DataItem (*DataParsingCallback)(int , uint8_t);
        typedef void (*DataChangedCallback)(DataItem);

    private:
        FirebaseConfig config;
        FirebaseData fbdo;
        FirebaseAuth auth;
        TaskHandle_t _dataChangeHandle;

        static DataParsingCallback dataParsingCallback;
        static FirebaseData stream;
        // create queue to detect if data is changed
        static QueueHandle_t queueFlagChangedData;
        static void streamCallback(FirebaseStream data);
        static void streamTimeoutCallback(bool timeout);
        static void onDataChangedEvent(DataChangedCallback* callback);

    public:
        void init();
        void start(int maxQueueSize);
        void stop();
        void registerDataChangeTask(DataChangedCallback* callback);

        void storeInt(const char* key, int value);
        void storeBool(const char* key, bool value);
        
        static void setDataParsingCallback(DataParsingCallback callback);
    
};