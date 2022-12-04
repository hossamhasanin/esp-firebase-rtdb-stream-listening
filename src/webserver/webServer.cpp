#include "webServer.h"
#include <Arduino.h>
#include <ArduinoJson.h>

static httpd_handle_t server = NULL;
static OnCredintialsSet onCredintialsSet = NULL;
static WebServer::OnOfflineDataChangedCallback onDataChangedCallback = NULL;
static GetDevicesAsJsonString getDevicesAsJsonString = NULL;

/* An HTTP GET handler */
static esp_err_t checkConnectedGetHandler(httpd_req_t* req){

    Serial.println("Esp connected to wifi successfully");
    httpd_resp_send(req, "Connected", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static const httpd_uri_t checkConnected = {
    .uri       = "/check_connected",
    .method    = HTTP_GET,
    .handler   = checkConnectedGetHandler,
    .user_ctx  = NULL
};


static esp_err_t wifiCredintialsPostHandler(httpd_req_t *req)
{
    /* Destination buffer for content of HTTP POST request.
     * httpd_req_recv() accepts char* only, but content could
     * as well be any binary data (needs type casting).
     * In case of string data, null termination will be absent, and
     * content length would give length of string */
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        /* In case of error, returning ESP_FAIL will
         * ensure that the underlying socket is closed */
        return ESP_FAIL;
    }


    Serial.println("recieved data");
    Serial.println(content);

    // Parse JSON object
    StaticJsonDocument<32> doc;
    DeserializationError error = deserializeJson(doc, content);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return ESP_FAIL;
    }
    const char* ssid = doc["ssid"];
    const char* password = doc["password"];

    Serial.print("wifiCredintialsPostHandler ");
    Serial.print("ssid: ");
    Serial.println(ssid);
    Serial.print("password: ");
    Serial.println(password);

    onCredintialsSet(ssid, password);

    return ESP_OK;
}

static const httpd_uri_t wifiCredintials = {
    .uri       = "/connect",
    .method    = HTTP_POST,
    .handler   = wifiCredintialsPostHandler,
    .user_ctx  = NULL
};


static esp_err_t webSocketHandler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        Serial.println("Handshake done, the new connection was opened");

        // std::string devices = getDevicesAsJsonString();
        // httpd_ws_frame_t ws_pkt;    
        // ws_pkt.payload = (uint8_t*)devices.c_str();
        // ws_pkt.len = devices.length();
        // ws_pkt.type = HTTPD_WS_TYPE_TEXT;

        // return httpd_ws_send_frame(req, &ws_pkt);

        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        Serial.println("httpd_ws_recv_frame failed to get frame len with");
        return ret;
    }
    Serial.printf("\n frame len is %d" , ws_pkt.len);

    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = (uint8_t*) calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            Serial.println("Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            Serial.println("httpd_ws_recv_frame failed with %d");
            free(buf);
            return ret;
        }
        Serial.printf("\n Got packet with message: %s", ws_pkt.payload);
    }
    Serial.printf("Packet type: %d", ws_pkt.type);
    /* parese the received data
        Data format: <device_index>_<state>
        ex: 0_35
        ex: 1_0
    */

   if (ws_pkt.type == HTTPD_WS_TYPE_TEXT){
        char* token = strtok((char*)ws_pkt.payload, "_");
        int device_index = atoi(token);
        token = strtok(NULL, "_");
        int state = atoi(token);
        Serial.printf("\n device_index: %d, state: %d", device_index, state);

        onDataChangedCallback(device_index, state);

        WebServer::sendChangesToWebSocketAsync(device_index, state);

        return ESP_OK;
   }
    
    // if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
    //     strcmp((char*)ws_pkt.payload,"Trigger async") == 0) {
    //     free(buf);
    //     return trigger_async_send(req->handle, req);
    // }

    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
        Serial.println("httpd_ws_send_frame failed with %d");
    }
    free(buf);
    return ret;
}

static const httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = webSocketHandler,
        .user_ctx   = NULL,
        .is_websocket = true
};

struct async_resp_arg {
    // httpd_handle_t hd;
    int fd;

    int device_index;
    int state;
};

/*
 * async send function, which we put into the httpd work queue
 */
static void ws_async_send(void *arg)
{

    Serial.println("\n sendChangesToWebSocketAsync");
    char data[32];
    struct async_resp_arg* resp_arg = (async_resp_arg*) arg;
    // httpd_handle_t hd = resp_arg->hd;
    // int fd = resp_arg->fd;

    sprintf(data, "{\"device\":\"%d_%d\"}", resp_arg->device_index, resp_arg->state);
    Serial.println(data);
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    
    httpd_ws_send_frame_async(server , resp_arg->fd, &ws_pkt);
    free(resp_arg);
}


void WebServer::sendChangesToWebSocketAsync(int device_index, int state)
{

    size_t len = MAX_SOCKET_CLIENTS;
    int socketClints[MAX_SOCKET_CLIENTS];
    ESP_ERROR_CHECK(httpd_get_client_list(server, &len, socketClints));

    for (int i = 0; i < len; i++) {
        Serial.printf("Sending to socket %d", socketClints[i]);
        struct async_resp_arg *resp_arg = (async_resp_arg*) malloc(sizeof(struct async_resp_arg));
        resp_arg->device_index = device_index;
        resp_arg->state = state;
        resp_arg->fd = socketClints[i];
        ESP_ERROR_CHECK(httpd_queue_work(server, ws_async_send, resp_arg));
    }
}

void WebServer::setOnOfflineDataChangedCallback(OnOfflineDataChangedCallback callback)
{
    onDataChangedCallback = callback;
}

void WebServer::setGetDevicesAsJsonStringCallback(GetDevicesAsJsonString callback)
{
    getDevicesAsJsonString = callback;
}

void WebServer::startWebServer(OnCredintialsSet callback){

    onCredintialsSet = callback;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.max_open_sockets = MAX_SOCKET_CLIENTS;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    Serial.print("Starting server on port:");
    Serial.println(config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        httpd_register_uri_handler(server, &checkConnected);
        httpd_register_uri_handler(server, &wifiCredintials);
        httpd_register_uri_handler(server, &ws);
        Serial.println("Registered URI handlers");
    } else {
            ESP_LOGI(TAG, "Error starting server!");
            Serial.println("Error starting server!");
    }

}

esp_err_t WebServer::stopWebServer(){
    httpd_unregister_uri(server, "/check_connected");
    httpd_unregister_uri(server, "/connect");
    httpd_unregister_uri(server, "/ws");
    return httpd_stop(server);
}