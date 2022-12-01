#include "wifiManager.h"
#include "data/nvsManager/nvsManager.h"


static IPAddress local_ip(192,168,1,222);
static IPAddress gateway(192,168,1,1);
static IPAddress subnet(255,255,255,0);
static IPAddress dns(192,168,1,1);

QueueHandle_t WiFiManager::isWifiOnQueuHanle = xQueueCreate(1, sizeof(bool));
static char* connectedPassword;

// static WiFiManager::OnSuccessfullyConncetedToWifi onSuccessfullyConncetedToWifiCallback = NULL;
// static WiFiManager::OnWifiConnectionLost onWifiConnectionLostCallback = NULL;

void storeWifiCredentialsToStorage(const char* ssid, const char* password){
    nvs_handle_t nvsHandle = NvsManager::openStorage();

    std::string ssidString(ssid);
    std::string passwordString(password);

    std::string storedSsid = NvsManager::getStringVal(nvsHandle, "ssid");
    std::string storedPassword = NvsManager::getStringVal(nvsHandle, "password");

    if (storedSsid == ssidString && storedPassword == passwordString){
        Serial.println("ssid already stored");
        NvsManager::closeStorage(nvsHandle);
        return;
    }

    Serial.println("Storage opened");
    NvsManager::storeString(nvsHandle, "ssid" , ssid);
    Serial.println("ssid stored");
    NvsManager::storeString(nvsHandle, "password" ,password);
    Serial.println("password stored");
    NvsManager::closeStorage(nvsHandle);
    Serial.println("Storage closed");
}

void WiFiManager::setupWifi(){


    nvs_handle_t nvsHandle = NvsManager::openStorage();

    if (nvsHandle == 0){
        Serial.println("Error opening storage");
        return;
    }

    std::string ssid = NvsManager::getStringVal(nvsHandle , "ssid");
    std::string password = NvsManager::getStringVal(nvsHandle , "password");

    if (ssid.empty() || password.empty()){
        Serial.println("No stored wifi credentials");
        WiFiManager::startAPMode();
    } else {
        Serial.println("Found stored wifi credentials");
        WiFiManager::startStationMode(ssid.c_str(), password.c_str());
    }

    
    NvsManager::closeStorage(nvsHandle);
}

void WiFiManager::startAPMode(){
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, local_ip, subnet , dns);
    WiFi.softAP(ESP_WIFI_SSID, ESP_WIFI_PASS);

    Serial.print("[+] AP Created with IP Gateway ");
    Serial.println(WiFi.softAPIP());
}

void staConnected(arduino_event_id_t event, arduino_event_info_t info){
    Serial.println("");
    Serial.print("[+] Connected to ssid");
}

void gotIp(arduino_event_id_t event, arduino_event_info_t info){

    if (WiFi.status() == WL_CONNECTED){
        Serial.println("");
        Serial.print("[+] IP address: ");
        Serial.println(WiFi.localIP());
        bool isWifiOn = true;
        if (xQueueSend(WiFiManager::isWifiOnQueuHanle, &isWifiOn, (TickType_t) 2) != pdPASS){
             Serial.println((const char *)FPSTR("stream queue full"));
        }
    } else {
        Serial.println("Error connecting to wifi");
    }
}

int connectRetries = 0;
void staDisconnected(arduino_event_id_t event, arduino_event_info_t info){
    Serial.println("[+] Station disconnected");

    connectRetries++;
    if (connectRetries > 2){
        bool isWifiOn = false;
        if (xQueueSend(WiFiManager::isWifiOnQueuHanle, &isWifiOn, (TickType_t) 2) != pdPASS){
             Serial.println((const char *)FPSTR("stream queue full"));
        }
        Serial.println("[+] Too many connection retries, starting AP mode");
        WiFiManager::startAPMode();

        connectRetries = 0;
    }

}

void WiFiManager::startStationMode(const char* ssid, const char* password){

    WiFi.mode(WIFI_STA);
    WiFi.config(local_ip, gateway, subnet , dns);
    WiFi.begin(ssid, password);

    Serial.print("[+] Connecting to ");
    Serial.println(ssid);

    storeWifiCredentialsToStorage(ssid, password);

    WiFi.onEvent(staConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(gotIp, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(staDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

}