#ifndef zrak_api_esp8266_h
#define zrak_api_esp8266_h

#include <Arduino.h>
#include <base64.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>

class zrak_client {
    public:
    struct http_response {
        int code;
        String response;
    };
    zrak_client(String username, String password, String host, boolean https);
    zrak_client(String username, String password, String host, char *fingerprint);
    void addVariable(String var_name, String var_value);
    void addVariable(String var_name, float var_value);
    void addVariable(String var_name, int32_t var_value);
    void printPayload();
    http_response send(String device_name);
    http_response send(int device_id);
    private:
    String _url_name = "/measurements?device_name=";
    String _url_id = "/measurements?device_id=";
    String _host;
    String _user_agent = "ESP8266_zrak_lib";
    String _username;
    String _password;
    String _payload = "{";
    base64 _b64;
    boolean _https;
    WiFiClientSecure _client_secure;
    WiFiClient _client;
};

#endif
