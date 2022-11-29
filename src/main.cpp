#include <Arduino.h>

#include "firebaseListener.h"
#include "data/dataHolder.h"
#include "data/nvsManager/nvsManager.h"

#include "webServer/webServer.h"
#include "wifiManager/wifiManager.h"

DataHolder data;
FirebaseListener firebaseListener;

void ifFirstTimeUp(){
    nvs_handle_t nvsHandle = NvsManager::openStorage();
    if(NvsManager::getU8(nvsHandle , "isFirstTime") == 0){
        NvsManager::storeU8(nvsHandle , "isFirstTime" , true);
        
        // set the default values to firebase
        firebaseListener.storeInt(String(temprature).c_str() , 0);
        firebaseListener.storeBool(String(switch1).c_str() , false);
    }
    NvsManager::closeStorage(nvsHandle);
}

void setup() {

  NvsManager::initNvsMemory();

  WiFiManager::setupWifi([](){
      Serial.println("Connected to wifi");
        
      firebaseListener.start();
    }, [](){
        Serial.println("Lost wifi connection");
        firebaseListener.stop();
    }
  );

  WebServer::startWebServer(WiFiManager::startStationMode);


  firebaseListener.init(DATA_FIELDS_COUNT);
  
  FirebaseListener::setDataParsingCallback([](const char* key, const char* value) -> DataItem {
    return data.parseDataFromKeyValue(key, value);
  });

  firebaseListener.registerDataChangeTask([](DataItem dataItem) {
    Serial.printf("Data changed: key: %d, value: %d", dataItem.key, (int) dataItem.value);
  });

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}