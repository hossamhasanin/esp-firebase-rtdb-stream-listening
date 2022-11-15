#include <Firebase_ESP_Client.h>
#include "freertos/FreeRTOS.h"
#include "data/dataItem.h"

/* 2. Define the API Key */
#define API_KEY "API_KEY"

/* 3. Define the RTDB URL */
#define DATABASE_URL "URL" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define DATA_PATH "/states"





class FirebaseListener{

    
    public:
        typedef DataItem (*DataParsingCallback)(const char* , const char*);
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
        static void onDataChangedEvent(DataChangedCallback callback);

    public:
        void init(int maxQueueSize);
        void start();
        void stop();
        void registerDataChangeTask(DataChangedCallback callback);

        void storeInt(const char* key, int value);
        void storeBool(const char* key, bool value);
        
        static void setDataParsingCallback(DataParsingCallback callback);
    
};