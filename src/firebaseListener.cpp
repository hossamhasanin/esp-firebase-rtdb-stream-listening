#include "firebaseListener.h"

QueueHandle_t FirebaseListener::queueFlagChangedData = NULL;
FirebaseData FirebaseListener::stream;
FirebaseListener::DataParsingCallback FirebaseListener::dataParsingCallback = NULL;

void FirebaseListener::init(int maxQueueSize) {
  
//   /* Assign the api key (required) */
//   config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */


  config.database_url = DATABASE_URL;

  config.signer.test_mode = true;

  // Firebase.reconnectWiFi(true);

    

  queueFlagChangedData = xQueueCreate(maxQueueSize, sizeof(DataItem));
}


void FirebaseListener::start() {

  /* Initialize the library with the Firebase authen and config */
  Firebase.begin(&config, &auth);

  if (!Firebase.RTDB.beginStream(&stream, DATA_PATH)) 
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());
  
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
  Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
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
            DataItem item = dataParsingCallback(value.key.c_str(), value.value.c_str());
            
            if (xQueueSend(queueFlagChangedData, (void *) &item, (TickType_t) 2) != pdPASS){
              Serial.println((const char *)FPSTR("stream queue full"));
        }
        }
        json->iteratorEnd();
        json->clear();

        
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

void FirebaseListener::onDataChangedEvent(DataChangedCallback callback) {
  DataItem dataChanged;
  if (xQueueReceive(queueFlagChangedData, &dataChanged, (TickType_t) 2) == pdPASS){
    if (dataChanged.key != NULL) {
      // Do something here
      Serial.println((const char *)FPSTR("Some data has been changed"));
      callback(dataChanged);
    }
  }
}

void FirebaseListener::registerDataChangeTask(DataChangedCallback callback) {
  xTaskCreate(
    [](void *param) {
      while (true) {
        DataChangedCallback* callback = (DataChangedCallback*) param;
        FirebaseListener::onDataChangedEvent(*callback);
      }
    },
    "dataChangeTask",
    1024,
    &callback,
    1,
    &_dataChangeHandle
  );
}

void FirebaseListener::setDataParsingCallback(DataParsingCallback callback) {
  FirebaseListener::dataParsingCallback = callback;
}

void FirebaseListener::stop() {
  Firebase.RTDB.endStream(&stream);
  // delete task
  // vTaskDelete(_dataChangeHandle);
  // xQueueReset(queueFlagChangedData);
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