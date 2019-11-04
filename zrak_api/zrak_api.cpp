#include "zrak_api.h"

zrak_client::zrak_client(String username, String password) {
    _username = username;
    _password = password;
}

void zrak_client::addVariable(String var_name, String var_value) {
    _payload += "\""+ var_name + "\": " + "\""+ var_value + "\", ";
}

void zrak_client::addVariable(String var_name, float var_value) {
    _payload += "\""+ var_name + "\": " + String(var_value) + ", ";
}

void zrak_client::addVariable(String var_name, int32_t var_value) {
    _payload += "\""+ var_name + "\": " + String(var_value) + ", ";
}

void zrak_client::printPayload() {
    Serial.println(_payload);
}

zrak_client::http_response zrak_client::send(String device_name) {
    String url = _url_name + device_name;
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    String auth = "Basic " + _b64.encode(_username+":"+_password);
    http.addHeader("Authorization", auth);
    _payload.remove(_payload.lastIndexOf(","));
    _payload += "}";
    int res_code = http.POST(_payload);
    http_response res;
    res.code = res_code;
    res.response = http.getString();
    _payload = "{";
    http.end();
    return res;
}

zrak_client::http_response zrak_client::send(int device_id) {
    String url = _url_id + device_id;
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    String auth = "Basic " + _b64.encode(_username+":"+_password);
    http.addHeader("Authorization", auth);
    _payload.remove(_payload.lastIndexOf(","));
    _payload += "}";
    int res_code = http.POST(_payload);
    http_response res;
    res.code = res_code;
    res.response = http.getString();
    _payload = "{";
    http.end();
    return res;
}