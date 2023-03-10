#include <Arduino.h>

#include "firebaseListener.h"
#include "data/dataHolder.h"
#include "data/nvsManager/nvsManager.h"

#include "webServer/webServer.h"
#include "wifiManager/wifiManager.h"
#include "freertos/FreeRTOS.h"
#include "uart/uartManager.h"
#include <WiFi.h>
#include <string>
#include <esp_task_wdt.h>

DataHolder data;
FirebaseListener firebaseListener;
UartManager uartManager;
FirebaseListener::DataChangedCallback onlineDataChangedCallback = [](DataItem dataItem) {
    Serial.printf("\n Online data changed: key: %d, value: %d\n", dataItem.key, (int) dataItem.value);

    data.set(dataItem.key, dataItem.value);

    //TODO: here send the changed data to microcontroller using serial
    uartManager.sendData(dataItem);
};
WebServer::OnOfflineDataChangedCallback offlineDataChangedCallback = [](int key, int value) {
    Serial.printf("\n Offline data changed: key: %d, value: %d", key, value);

    data.set(key, value);

};

UartManager::DataChangedCallback uartDataChangedCallback = [](DataItem dataItem) {
    Serial.printf("\n uart data changed: key: %d, value: %d\n", dataItem.key, (int) dataItem.value);

    data.set(dataItem.key, dataItem.value);

    //here send the changed data to firbase
    firebaseListener.saveField(dataItem);
    
};

GetDevicesAsJsonString getDevicesAsJsonString = []() -> const char* {
    std::string devices = "{\"devices\":[/s]}";

    for (int i = 0; i < DATA_FIELDS_COUNT; i++) {
        char device[32];
        sprintf(device, "%d_%d", i, data.get(i));
        if (i != DATA_FIELDS_COUNT - 1) {
            strcat(device, ",/s");
        }
        devices.replace(devices.find("/s"), 2, device);
    }

    return devices.c_str();
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
    firebaseListener.saveField(data.getDataItem(doorState));
    firebaseListener.saveField(data.getDataItem(temp));
    firebaseListener.saveField(data.getDataItem(led1));
    firebaseListener.saveField(data.getDataItem(electri));
    firebaseListener.saveField(data.getDataItem(rgblState));
    firebaseListener.saveField(data.getDataItem(gasLeakAlarm));
    firebaseListener.saveField(data.getDataItem(numOfPeople));
    firebaseListener.saveField(data.getDataItem(passwordWrongAlarm));
    firebaseListener.saveField(data.getDataItem(powerConsumption));
}

void startFirebaseTask(void* parameter){
    while(true){
      bool isWifiOn;
      if (xQueueReceive(WiFiManager::isWifiOnQueuHanle, &isWifiOn, (TickType_t) 2) == pdPASS){
        if(isWifiOn){

          Serial.println("Wifi connected");
          // ESP_ERROR_CHECK(WebServer::stopWebServer());  
          WebServer::stopWebSocket();
            
          firebaseListener.init();
          
          firebaseListener.start(DATA_FIELDS_COUNT);
          firebaseListener.registerDataChangeTask(&onlineDataChangedCallback);
          firebaseListener.registerTimerToUpdateLastOnline();
          sendDevicesStateToFirebase();
        } else {
          Serial.println("Wifi disconnected");

          firebaseListener.stop();
          WebServer::startWebServer(WiFiManager::startStationMode);
        }
      }
    }
}



void setup() {

  Serial.begin(115200);

  // remove comment
  // NvsManager::initNvsMemory();

  esp_task_wdt_init(30, false);

  WiFi.mode(WIFI_STA);
  WiFi.config(IPAddress(192,168,1,222), IPAddress(192,168,1,1), IPAddress(255,255,255,0) , IPAddress(192,168,1,1));
  WiFi.begin("SilliconVally" , "Qwerty@013008$8720$hgfa$annie$olaf$2003$@");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  firebaseListener.init();        
  FirebaseListener::setDataParsingCallback([](int key, uint8_t value) -> DataItem {
    return data.parseDataFromKeyValue(key, value);
  });
  firebaseListener.start(DATA_FIELDS_COUNT);
  firebaseListener.registerDataChangeTask(&onlineDataChangedCallback);
  // sendDevicesStateToFirebase();


  // remove comment
  // FirebaseListener::setDataParsingCallback([](int key, uint8_t value) -> DataItem {
  //           return data.parseDataFromKeyValue(key, value);
  // });

  // remove comment
  // create task to handle firebase
  // xTaskCreate(
  //   startFirebaseTask, /* Task function. */
  //   "startFirebaseTask", /* name of task. */
  //   1024*6, /* Stack size of task */
  //   NULL, /* parameter of the task */
  //   1, /* priority of the task */
  //   NULL /* Task handle to keep track of created task */);

  // WiFiManager::setupWifi();

  // WebServer::setOnOfflineDataChangedCallback(offlineDataChangedCallback);

  // WebServer::setGetDevicesAsJsonStringCallback(getDevicesAsJsonString);




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

  uartManager.initUart();
  uartManager.registerDataChangedCallback(&uartDataChangedCallback);
  // vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // remove comment
  // firebaseListener.updateTimaStamp();

  // char* test_str = "Hi\n";
  // uart_write_bytes(EX_UART_NUM, (const char*)test_str, strlen(test_str));
  // Serial.println("Sent to uart");
  // delay(1000);
}