#pragma once

#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>

#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include <string>

typedef void (*OnCredintialsSet)(const char*, const char*);
typedef const char* (*GetDevicesAsJsonString)();


#define MAX_SOCKET_CLIENTS 10
#define CONFIG_HTTPD_WS_SUPPORT 1


namespace WebServer {    

    typedef void (*OnOfflineDataChangedCallback)(int, int);

    void registerHandlers();

    void startWebServer(OnCredintialsSet callback);

    void setOnOfflineDataChangedCallback(OnOfflineDataChangedCallback callback);

    void setGetDevicesAsJsonStringCallback(GetDevicesAsJsonString callback);

    void sendChangesToWebSocketAsync(int key, int value);

    esp_err_t stopWebServer();
}