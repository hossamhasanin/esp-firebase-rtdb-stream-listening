#include "firebaseListener.h"

QueueHandle_t FirebaseListener::queueFlagChangedData = NULL;
FirebaseData FirebaseListener::stream;
FirebaseListener::DataParsingCallback FirebaseListener::dataParsingCallback = NULL;

void FirebaseListener::init() {
  
//   /* Assign the api key (required) */
//   config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */


  config.database_url = DATABASE_URL;

  config.signer.test_mode = true;

  // Firebase.reconnectWiFi(true);

}


void FirebaseListener::start(int maxQueueSize) {

  queueFlagChangedData = xQueueCreate(maxQueueSize, sizeof(DataItem));

  /* Initialize the library with the Firebase authen and config */

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  if (!Firebase.RTDB.beginStream(&stream, DATA_PATH)) {
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());
  }
  
  // Set the callback function for the event and timeout.
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
}



void FirebaseListener::streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void FirebaseListener::streamCallback(FirebaseStream data)
{
  Serial.printf("stream path %s , \n event path %s , \n data type %s, \n event type %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json){
        FirebaseJson *json = data.to<FirebaseJson *>();

        
        // Print all object data
        Serial.println((const char *)FPSTR("Pretty printed JSON data:"));
        json->toString(Serial, true);
        Serial.println();
        Serial.println((const char *)FPSTR("Iterate JSON data:"));
        Serial.println();
        size_t len = json->iteratorBegin();
        FirebaseJson::IteratorValue value;
        
        for (size_t i = 0; i < len; i++)
        {
            value = json->valueAt(i);
            Serial_Printf((const char *)FPSTR("%d, Type: %s, Name: %s, Value: %s\n"), i, value.type == FirebaseJson::JSON_OBJECT ? (const char *)FPSTR("object") : (const char *)FPSTR("array"), value.key.c_str(), value.value.c_str());
            // DataItem item = dataParsingCallback(i, value.value.c_str());
            
            // if (xQueueSend(queueFlagChangedData, (void *) &item, (TickType_t) 2) != pdPASS){
            //   Serial.println((const char *)FPSTR("stream queue full"));
            // }
        }
        json->iteratorEnd();
        json->clear();

        
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_array) {
    FirebaseJsonArray *arr = data.to<FirebaseJsonArray *>();
    Serial.println((const char *)FPSTR("Pretty printed JSON array:"));
    arr->toString(Serial, true);
    Serial.println();
    Serial.println((const char *)FPSTR("Iterate JSON array:"));
    Serial.println();
    size_t len = arr->iteratorBegin();
    FirebaseJson::IteratorValue value;
    for (size_t i = 0; i < len; i++)
    {
        value = arr->valueAt(i);
        Serial_Printf((const char *)FPSTR("%d, Type: %s, Value: %s\n"), i, value.type == FirebaseJson::JSON_OBJECT ? (const char *)FPSTR("object") : (const char *)FPSTR("array"), value.value.c_str());

        // DataItem item = FirebaseListener::dataParsingCallback(i, value.value.c_str());
        
        // Serial_Printf((const char *)FPSTR("item key %d, value %d\n"), item.key, item.value);

        // if (xQueueSend(queueFlagChangedData, (void *) &item, (TickType_t) 2) != pdPASS){
        //   Serial.println((const char *)FPSTR("stream queue full"));
        // }
    }
    arr->iteratorEnd();
    arr->clear();
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
    // extract the key from "/1" data path
    int key = atoi(data.dataPath().c_str() + 1);
    DataItem item = FirebaseListener::dataParsingCallback(key, data.intData());    
    Serial_Printf((const char *)FPSTR("item key %d, value %d\n"), item.key, item.value);
    if (xQueueSend(queueFlagChangedData, (void *) &item, (TickType_t) 2) != pdPASS){
        Serial.println((const char *)FPSTR("stream queue full"));
    }
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_boolean) {
    int key = atoi(data.dataPath().c_str() + 1);
    DataItem item = FirebaseListener::dataParsingCallback(key, data.boolData());    
    Serial_Printf((const char *)FPSTR("item key %d, value %d\n"), item.key, item.value);
    if (xQueueSend(queueFlagChangedData, (void *) &item, (TickType_t) 2) != pdPASS){
        Serial.println((const char *)FPSTR("stream queue full"));
    }
  } else {
    Serial.println((const char *)FPSTR("Data type is not JSON"));
  }

  // This is the size of stream payload received (current and max value)
  // Max payload size is the payload size under the stream path since the stream connected
  // and read once and will not update until stream reconnection takes place.
  // This max value will be zero as no payload received in case of ESP8266 which
  // BearSSL reserved Rx buffer size is less than the actual stream payload.
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", data.payloadLength(), data.maxPayloadLength());

  // Due to limited of stack memory, do not perform any task that used large memory here especially starting connect to server.
  // Just set this flag and check it status later.
}

void FirebaseListener::onDataChangedEvent(DataChangedCallback* callback) {
  DataItem dataChanged;
  if (xQueueReceive(queueFlagChangedData, &dataChanged, (TickType_t) 2) == pdPASS){
    // if (dataChanged.key != NULL) {
      // Do something here
      Serial.println((const char *)FPSTR("Some data has been changed"));
      Serial.println(dataChanged.key);
      Serial.println(dataChanged.value);
      (*callback)(dataChanged);
    // }
  }
}

void FirebaseListener::registerDataChangeTask(DataChangedCallback* callback) {
  xTaskCreate(
    [](void *param) {
      while (true) {
        DataChangedCallback* c = (DataChangedCallback*) param;
        if (queueFlagChangedData != NULL) {
          FirebaseListener::onDataChangedEvent(c);
        }
      }
    },
    "dataChangeTask",
    1024*2,
    callback,
    3,
    &_dataChangeHandle
  );
}

void FirebaseListener::setDataParsingCallback(DataParsingCallback callback) {
  FirebaseListener::dataParsingCallback = callback;
}

void FirebaseListener::stop() {
  Firebase.RTDB.endStream(&stream);
  // delete task
  vTaskDelete(_dataChangeHandle);
  xQueueReset(queueFlagChangedData);
}

void FirebaseListener::storeInt(const char* key, int value) {
  // concatinate DATA_PATH , "/" and the key
  char path[strlen(DATA_PATH) + strlen(key) + 2];
  strcpy(path, DATA_PATH);
  strcat(path, "/");
  strcat(path, key);
  Firebase.RTDB.setInt(&fbdo, path, value);
}

void FirebaseListener::storeBool(const char* key, bool value) {
  // concatinate DATA_PATH , "/" and the key
  char path[strlen(DATA_PATH) + strlen(key) + 2];
  strcpy(path, DATA_PATH);
  strcat(path, "/");
  strcat(path, key);
  Firebase.RTDB.setBool(&fbdo, path, value);
}