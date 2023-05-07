#include "firebaseListener.h"

QueueHandle_t FirebaseListener::queueFlagChangedData = NULL;
TaskHandle_t FirebaseListener::_timerHandle = NULL;
SemaphoreHandle_t FirebaseListener::timerSem = NULL;
DevicesManager* FirebaseListener::data = nullptr;

FirebaseData FirebaseListener::stream;
FirebaseData FirebaseListener::fbdo;
// FirebaseListener::DataParsingCallback FirebaseListener::dataParsingCallback = NULL;

void FirebaseListener::setupFirebaseFactory(DevicesManager* dataHolder , DataChangedCallback* dataChangedCallback , int dataFildsCount) {
  this->init(dataHolder);
  this->start(dataFildsCount);
  this->registerDataChangeTask(dataChangedCallback);
  this->registerTimerToUpdateLastOnline();
}

void FirebaseListener::init(DevicesManager* data) {
  FirebaseListener::data = data;
  
//   /* Assign the api key (required) */
//   config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */


  config.database_url = DATABASE_URL;

  config.signer.test_mode = true;

  // Firebase.reconnectWiFi(true);

}


void FirebaseListener::start(int maxQueueSize) {

  queueFlagChangedData = xQueueCreate(maxQueueSize, sizeof(uint8_t));

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
    // split data.dataPath() by "/"
    // extract the last element the field name /1/isOn
    
    const char* field = data.dataPath().substring(data.dataPath().lastIndexOf("/") + 1).c_str();

    Serial.printf("key %d \n" , key);
    Serial.printf("field %c \n" , field);
    DeviceStateHolder stateHolder;
    stateHolder.fieldName = field;
    stateHolder.intValue = data.intData();
    FirebaseListener::data->getDevice(key)->updatedDeviceState(stateHolder);
    Serial_Printf((const char *)FPSTR("item key %d, value %d\n"), key, data.intData());
    notifyDataChangedToQueue(key);
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_double) {
    int key = atoi(data.dataPath().c_str() + 1);
    const char* field = data.dataPath().substring(data.dataPath().lastIndexOf("/") + 1).c_str();

    Serial.printf("key %d \n" , key);
    Serial.printf("field %c \n" , field);
    DeviceStateHolder stateHolder;
    stateHolder.fieldName = field;
    stateHolder.doubleValue = data.doubleData();
    FirebaseListener::data->getDevice(key)->updatedDeviceState(stateHolder);
    Serial_Printf((const char *)FPSTR("item key %d, value %d\n"), key, data.doubleData());
    notifyDataChangedToQueue(key);
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_float) {
    int key = atoi(data.dataPath().c_str() + 1);
    const char* field = data.dataPath().substring(data.dataPath().lastIndexOf("/") + 1).c_str();

    Serial.printf("key %d \n" , key);
    Serial.printf("field %c \n" , field);
    DeviceStateHolder stateHolder;
    stateHolder.fieldName = field;
    stateHolder.doubleValue = data.floatData();
    FirebaseListener::data->getDevice(key)->updatedDeviceState(stateHolder);
    Serial_Printf((const char *)FPSTR("item key %d, value %d\n"), key, data.floatData());
    notifyDataChangedToQueue(key);
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_boolean) {
    int key = atoi(data.dataPath().c_str() + 1);
    const char* field = data.dataPath().substring(data.dataPath().lastIndexOf("/") + 1).c_str();

    Serial.printf("key %d \n" , key);
    Serial.printf("field %c \n" , field);
    DeviceStateHolder stateHolder;
    stateHolder.fieldName = field;
    stateHolder.boolValue = data.boolData();
    FirebaseListener::data->getDevice(key)->updatedDeviceState(stateHolder);
    Serial_Printf((const char *)FPSTR("item key %d, value %d\n"), key, data.boolData());
    notifyDataChangedToQueue(key);
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

void FirebaseListener::notifyDataChangedToQueue(uint8_t dataKey){
  if (xQueueSend(queueFlagChangedData, (void *) &dataKey, (TickType_t) 2) != pdPASS){
        Serial.println((const char *)FPSTR("stream queue full"));
  }
}

void FirebaseListener::onDataChangedEvent(DataChangedCallback* callback) {
  uint8_t dataKey;
  if (xQueueReceive(queueFlagChangedData, &dataKey, (TickType_t) 2) == pdPASS){
    // if (dataChanged.key != NULL) {
      // Do something here
      Serial.println((const char *)FPSTR("Some data has been changed"));
      (*callback)(dataKey);
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

// void FirebaseListener::setDataParsingCallback(DataParsingCallback callback) {
//   FirebaseListener::dataParsingCallback = callback;
// }

bool IRAM_ATTR  FirebaseListener::timerCallback(void *param) {
  Serial.println((const char *)FPSTR("Update last online interrupt"));

    BaseType_t high_task_awoken = pdFALSE;
    xSemaphoreGiveFromISR(timerSem, &high_task_awoken);
    return (high_task_awoken == pdTRUE);
}



void FirebaseListener::registerTimerToUpdateLastOnline(){
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
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    // set alarm to 30 seconds
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 30000000);

    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, FirebaseListener::timerCallback, NULL, 0);
    timer_start(TIMER_GROUP_0, TIMER_0);

}


void FirebaseListener::stop() {

  // check if not initialized get out
  if (timerSem == nullptr) {
    return;
  }

  Firebase.RTDB.endStream(&stream);
  // delete task
  vTaskDelete(_dataChangeHandle);
  xQueueReset(queueFlagChangedData);

  // delete timer
  timer_disable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_callback_remove(TIMER_GROUP_0, TIMER_0);
  timer_pause(TIMER_GROUP_0, TIMER_0);
  timer_deinit(TIMER_GROUP_0, TIMER_0);
  vSemaphoreDelete(timerSem);
  timerSem = nullptr;
}

void FirebaseListener::updateTimaStamp(){
  if (timerSem != NULL){
    if (xSemaphoreTake(timerSem, portMAX_DELAY) == pdPASS) {
      Serial.println((const char *)FPSTR("Updating last online"));
      Firebase.RTDB.setTimestamp(&fbdo, LAST_ONLINE_PATH);
      Serial.println((const char *)FPSTR("Updated last online"));
    }
  }
}

void FirebaseListener::updateSwitch(Switch* sw) {
  String key = String(sw->getKey());
  char path[strlen(DATA_PATH) + 1 + strlen(key.c_str()) + 1 + strlen(IS_ON_FEATURE) + 1];
  strcpy(path, DATA_PATH);
  strcat(path, "/");
  strcat(path, key.c_str());
  strcat(path, "/");
  strcat(path, IS_ON_FEATURE);
  Serial.println(path);
  Firebase.RTDB.setBool(&fbdo, path, sw->getState());
}

void FirebaseListener::updateTempratureSensor(TempratureSensor* ts) {
  String key = String(ts->getKey());
  char path[strlen(DATA_PATH) + 1 + strlen(key.c_str()) + 1 + strlen(TEMPRATURE_FEATURE) + 1];
  strcpy(path, DATA_PATH);
  strcat(path, "/");
  strcat(path, key.c_str());
  strcat(path, "/");
  strcat(path, TEMPRATURE_FEATURE);
  Firebase.RTDB.setInt(&fbdo, path, ts->getTemprature());
}

void FirebaseListener::updatePowerConsumption(PowerConsumption* pc) {
  FirebaseJson json;
  json.set("value", pc->getPowerConsumption());
  Firebase.RTDB.pushJSON(&fbdo, POWER_CONSUMPTION_PATH, &json);
  // concatinate path with "/timestamp"
  char timestampPath[fbdo.dataPath().length() + fbdo.pushName().length() + strlen("/timestamp") + 2];
  strcpy(timestampPath, fbdo.dataPath().c_str());
  strcat(timestampPath, "/");
  strcat(timestampPath, fbdo.pushName().c_str());
  strcat(timestampPath, "/timestamp");
  Firebase.RTDB.setTimestamp(&fbdo, timestampPath);
}

void FirebaseListener::updatePeopleCounter(PeopleCounter* pc) {
  String key = String(pc->getKey());
  char path[strlen(DATA_PATH) + 1 + strlen(key.c_str()) + 1 + strlen(PEOPLE_COUNTER_FEATURE) + 1];
  strcpy(path, DATA_PATH);
  strcat(path, "/");
  strcat(path, key.c_str());
  strcat(path, "/");
  strcat(path, PEOPLE_COUNTER_FEATURE);
  Firebase.RTDB.setInt(&fbdo, path, pc->getPeopleCount());
}

void FirebaseListener::updateRgbLight(RgbLight* rgb) {
  String key = String(rgb->getKey());
  char path[strlen(DATA_PATH) + 1 + strlen(key.c_str()) + 1];
  strcpy(path, DATA_PATH);
  strcat(path, "/");
  strcat(path, key.c_str());
  FirebaseJson json;
  json.set(COLOR_ID_FEATURE , rgb->getColorId());
  json.set(IS_ON_FEATURE , rgb->getIsOn());
  Firebase.RTDB.setJSON(&fbdo, path, &json);
}

void FirebaseListener::updateAc(AcCommands* ac) {
  Serial.println((const char *)FPSTR("Updating ac not implemented yet"));
}




