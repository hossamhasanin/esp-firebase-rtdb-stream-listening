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
FirebaseListener::DataChangedCallback onlineDataChangedCallback = [](uint8_t key) {
    Serial.printf("\n Online data changed: key: %d\n", key);


    //TODO: here send the changed data to microcontroller using serial
    uartManager.sendData(key , data.getByteData(key));
};
WebServer::OnOfflineDataChangedCallback offlineDataChangedCallback = [](int key, int value) {
    Serial.printf("\n Offline data changed: key: %d, value: %d", key, value);

    // data.set(key, value);

};

UartManager::DataChangedCallback uartDataChangedCallback = [](uint8_t key) {
    Serial.printf("\nUart data changed: key: %d\n", key);

    //here send the changed data to firbase
    if (key == tempId){
      firebaseListener.storeInt(String(key).c_str() , data.getTemp());
    } else if (key == doorStateId){
      firebaseListener.storeBool(String(key).c_str() , data.getDoorState());
    } else if (key == led1Id){
      firebaseListener.storeBool(String(key).c_str() , data.getLed1());
    } else if (key == electriId){
      firebaseListener.storeBool(String(key).c_str() , data.getElectri());
    } else if (key == rgblStateId){
      firebaseListener.storeBool(String(key).c_str() , data.getRgblState());
    } else if (key == gasLeakAlarmId){
      firebaseListener.storeBool(String(key).c_str() , data.getGasLeakAlarm());
    } else if (key == numOfPeopleId){
      firebaseListener.storeInt(String(key).c_str() , data.getNumOfPeople());
    } else if (key == passwordWrongAlarmId){
      firebaseListener.storeBool(String(key).c_str() , data.getPasswordWrongAlarm());
    } else if (key == powerConsumptionId){
      Serial.printf("powerConsumption is to store: %f\n", data.getPowerConsumption());
      
      firebaseListener.storePowerConsumption(data.getPowerConsumption());
    }
    
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
    firebaseListener.storeBool(String(tempId).c_str() , data.getTemp());
    firebaseListener.storeBool(String(doorStateId).c_str() , data.getDoorState());
    firebaseListener.storeBool(String(led1Id).c_str() , data.getLed1());
    firebaseListener.storeBool(String(electriId).c_str() , data.getElectri());
    firebaseListener.storeBool(String(rgblStateId).c_str() , data.getRgblState());
    firebaseListener.storeBool(String(gasLeakAlarmId).c_str() , data.getGasLeakAlarm());
    firebaseListener.storeInt(String(numOfPeopleId).c_str() , data.getNumOfPeople());
    firebaseListener.storeBool(String(passwordWrongAlarmId).c_str() , data.getPasswordWrongAlarm());
    firebaseListener.storeDouble(String(powerConsumptionId).c_str() , data.getPowerConsumption());
}

void startFirebaseTask(void* parameter){
    while(true){
      bool isWifiOn;
      if (xQueueReceive(WiFiManager::isWifiOnQueuHanle, &isWifiOn, (TickType_t) 2) == pdPASS){
        if(isWifiOn){

          Serial.println("Wifi connected");
          // ESP_ERROR_CHECK(WebServer::stopWebServer());  
          WebServer::stopWebSocket();
            
          firebaseListener.init(&data);
          
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

  firebaseListener.init(&data);        
  // FirebaseListener::setDataParsingCallback([](int key, uint8_t value) -> DataItem {
  //   return data.parseDataFromKeyValue(key, value);
  // });
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

  uartManager.initUart(&data);
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