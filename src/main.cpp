#include <Arduino.h>

#include "data/nvsManager/nvsManager.h"

#include "webServer/webServer.h"
#include "wifiManager/wifiManager.h"
#include "freertos/FreeRTOS.h"
#include "uart/uartManager.h"
#include "firebaseListener.h"
#include <WiFi.h>
#include <string>
#include <esp_task_wdt.h>
#include "data/devices/devicesManager.h"

DevicesManager devicesManager;
FirebaseListener firebaseListener;
UartManager uartManager;
FirebaseListener::DataChangedCallback onlineDataChangedCallback = [](uint8_t key) {
    Serial.printf("\n Online data changed: key: %d\n", key);


    //here send the changed data to microcontroller using serial
    uartManager.updateDevice(devicesManager.getDevice(key));
};
WebServer::OnOfflineDataChangedCallback offlineDataChangedCallback = [](int key, int value) {
    Serial.printf("\n Offline data changed: key: %d, value: %d", key, value);

    // data.set(key, value);

};

UartManager::DataChangedCallback uartDataChangedCallback = [](uint8_t key) {
    Serial.printf("\nUart data changed: key: %d\n", key);

    //TODO: remember to check on the the wifi connection before sending data to firebase
    // If the wifi is not connected, the data will be sent to the web socket server
    // and will be sent to firebase when the wifi is connected again
    firebaseListener.updateDevice(devicesManager.getDevice(key));
};

GetDevicesAsJsonString getDevicesAsJsonString = []() -> const char* {
    std::string devices = "{\"devices\":[/s]}";

    // for (int i = 0; i < DATA_FIELDS_COUNT; i++) {
    //     char device[32];
    //     sprintf(device, "%d_%d", i, data.getDataItem(i).key);
    //     if (i != DATA_FIELDS_COUNT - 1) {
    //         strcat(device, ",/s");
    //     }
    //     devices.replace(devices.find("/s"), 2, device);
    // }

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
    for (auto const& x : *devicesManager.getDevices())
    {
        firebaseListener.updateDevice(x.second);
    }
}

void startFirebaseTask(void* parameter){
    while(true){
      bool isWifiOn;
      if (xQueueReceive(WiFiManager::isWifiOnQueuHanle, &isWifiOn, (TickType_t) 2) == pdPASS){
        if(isWifiOn){

          Serial.println("Wifi connected");
          // ESP_ERROR_CHECK(WebServer::stopWebServer());  
          WebServer::stopWebSocket();
            
          firebaseListener.setupFirebaseFactory(&devicesManager , &onlineDataChangedCallback , DATA_FIELDS_COUNT);

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

  esp_task_wdt_init(60, false);

  // WiFi.mode(WIFI_STA);
  // Home network
  WiFi.config(IPAddress(192,168,1,222), IPAddress(192,168,1,1), IPAddress(255,255,255,0) , IPAddress(192,168,1,1));
  // The G
  // WiFi.config(IPAddress(192,168,232,222), IPAddress(192,168,232,99), IPAddress(255,255,255,0) , IPAddress(192,168,232,99));
  // Saba wifi
  // WiFi.config(IPAddress(192,168,6,222), IPAddress(192,168,6,154), IPAddress(255,255,255,0) , IPAddress(192,168,6,154));
  // Home network
  WiFi.begin("SilliconVally" , "Qwerty@013008$8720$hgfa$annie$olaf$2003$@");
  // The G
  // WiFi.begin("The G" , "123456789hg");


  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  firebaseListener.setupFirebaseFactory(&devicesManager , &onlineDataChangedCallback , DATA_FIELDS_COUNT);


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



  // uartManager.setupUartFactory(&data, &uartDataChangedCallback);
  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // remove comment
  // firebaseListener.updateTimaStamp();

}