#ifndef zrak_api_esp32_h
#define zrak_api_esp32_h

#include <Arduino.h>
#include <base64.h>
#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#endif
#ifdef ESP32
#include <HTTPClient.h>
#endif

class zrak_client {
    public:
    struct http_response {
        int code;
        String response;
    };
    zrak_client(String username, String password);
    void addVariable(String var_name, String var_value);
    void addVariable(String var_name, float var_value);
    void addVariable(String var_name, int32_t var_value);
    void printPayload();
    http_response send(String device_name);
    http_response send(int device_id);
    private:
    String _url_name = "https://api.zrak.janvr.wtf/measurements?device_name=";
    String _url_id = "https://api.zrak.janvr.wtf/measurements?device_id=";
    String _username;
    String _password;
    String _payload = "{";
    base64 _b64;
};

#endif
