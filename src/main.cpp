#include <Arduino.h>

#include "firebaseListener.h"
#include "data/dataHolder.h"
#include "data/nvsManager/nvsManager.h"

#include "webServer/webServer.h"
#include "wifiManager/wifiManager.h"
#include "freertos/FreeRTOS.h"
#include <WiFi.h>

DataHolder data;
FirebaseListener firebaseListener;
FirebaseListener::DataChangedCallback dataChangedCallback = [](DataItem dataItem) {
    Serial.printf("Data changed: key: %d, value: %d", dataItem.key, (int) dataItem.value);
};

// void ifFirstTimeUp(){
//     nvs_handle_t nvsHandle = NvsManager::openStorage();
//     if(NvsManager::getU8(nvsHandle , "isFirstTime") == 0){
//         NvsManager::storeU8(nvsHandle , "isFirstTime" , true);
        
//         // set the default values to firebase
//         firebaseListener.storeInt(String(temprature).c_str() , 0);
//         firebaseListener.storeBool(String(switch1).c_str() , false);
//     }
//     NvsManager::closeStorage(nvsHandle);
// }

void sendDevicesStateToFirebase(){
    firebaseListener.storeInt(String(temprature).c_str() , data.get(temprature));
    firebaseListener.storeBool(String(switch1).c_str() , data.get(switch1));
}

void startFirebaseTask(void* parameter){
    while(true){
      bool isWifiOn;
      if (xQueueReceive(WiFiManager::isWifiOnQueuHanle, &isWifiOn, (TickType_t) 2) == pdPASS){
        if(isWifiOn){

          Serial.println("Wifi connected");
          ESP_ERROR_CHECK(WebServer::stopWebServer());          
          firebaseListener.init();
          
          firebaseListener.start(DATA_FIELDS_COUNT);
          firebaseListener.registerDataChangeTask(&dataChangedCallback);
          // sendDevicesStateToFirebase();
        } else {
          Serial.println("Wifi disconnected");
        }
      }
    }
}

void setup() {

  Serial.begin(115200);
  NvsManager::initNvsMemory();


  // WiFi.mode(WIFI_STA);
  // WiFi.config(IPAddress(192,168,1,222), IPAddress(192,168,1,1), IPAddress(255,255,255,0) , IPAddress(192,168,1,1));
  // WiFi.begin("SilliconVally" , "01005887324hgfaHossamG$$");

  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.print(".");
  //   delay(300);
  // }

  // firebaseListener.init();
  //     FirebaseListener::setDataParsingCallback([](int key, uint8_t value) -> DataItem {
  //       return data.parseDataFromKeyValue(key, value);
  //     });
  //     firebaseListener.start(DATA_FIELDS_COUNT);
  //     firebaseListener.registerDataChangeTask(&dataChangedCallback);

  FirebaseListener::setDataParsingCallback([](int key, uint8_t value) -> DataItem {
            return data.parseDataFromKeyValue(key, value);
  });
  // create task to handle firebase
  xTaskCreate(
    startFirebaseTask, /* Task function. */
    "startFirebaseTask", /* name of task. */
    1024*6, /* Stack size of task */
    NULL, /* parameter of the task */
    1, /* priority of the task */
    NULL /* Task handle to keep track of created task */);

  WiFiManager::setupWifi();

  WebServer::startWebServer(WiFiManager::startStationMode);



  // WiFiManager::setupWifi([](){
  //     Serial.println("Connected to wifi");
        
  //       firebaseListener.init();
  //       FirebaseListener::setDataParsingCallback([](int key, uint8_t value) -> DataItem {
  //         return data.parseDataFromKeyValue(key, value);
  //       });
  //       firebaseListener.start(DATA_FIELDS_COUNT);
  //       firebaseListener.registerDataChangeTask(&dataChangedCallback);

  //     // sendDevicesStateToFirebase();
  //   }, [](){
  //       Serial.println("Lost wifi connection");

  //       // firebaseListener.stop();
  //       // WebServer::startWebServer(WiFiManager::startStationMode);
  //   }
  // );




  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}