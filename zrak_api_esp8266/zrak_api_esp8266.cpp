#include "zrak_api_esp8266.h"

zrak_client::zrak_client(String username, String password, String host, boolean https)
{
    _username = username;
    _password = password;
    _host = host;
    _https = https;
    if (https)
        _client_secure.setInsecure();
}

zrak_client::zrak_client(String username, String password, String host, char *fingerprint)
{
    _username = username;
    _password = password;
    _host = host;
    _https = true;
    _client_secure.setFingerprint(fingerprint);
}

void zrak_client::addVariable(String var_name, String var_value)
{
    _payload += "\"" + var_name + "\": " + "\"" + var_value + "\", ";
}

void zrak_client::addVariable(String var_name, float var_value)
{
    _payload += "\"" + var_name + "\": " + String(var_value) + ", ";
}

void zrak_client::addVariable(String var_name, int32_t var_value)
{
    _payload += "\"" + var_name + "\": " + String(var_value) + ", ";
}

void zrak_client::printPayload()
{
    Serial.println(_payload);
}

zrak_client::http_response zrak_client::send(String device_name)
{

    _payload.remove(_payload.lastIndexOf(","));
    _payload += "}";

    http_response res;

    if (_https)
    {
        if (!_client_secure.connect(_host, 443))
        {
            res.code = -1;
            res.response = "Couldn't connect to host. Maybe wrong fingerprint?";
            return res;
        }

        _client_secure.println("POST " + _url_name + device_name + " HTTP/1.1");
        _client_secure.println("Host: " + _host);
        _client_secure.println("User-Agent: " + _user_agent);
        _client_secure.println("Content-Type: application/json");
        _client_secure.println("Content-Length: " + String(_payload.length()));
        _client_secure.println("Authorization: Basic " + _b64.encode(_username + ":" + _password));
        _client_secure.println();
        _client_secure.print(_payload);

        _client_secure.readStringUntil(' ');
        int status = _client_secure.readStringUntil(' ').toInt();

        while (_client_secure.connected())
        {
            String line = _client_secure.readStringUntil('\n');
            if (line == "\r")
                break;
        }

        String response = _client_secure.readString();

        res.code = status;
        res.response = response;

        _client_secure.stop();
    }
    else
    {
        if (!_client.connect(_host, 80))
        {
            res.code = -1;
            res.response = "Couldn't connect to host.";
            return res;
        }

        _client.println("POST " + _url_name + device_name + " HTTP/1.1");
        _client.println("Host: " + _host);
        _client.println("User-Agent: " + _user_agent);
        _client.println("Content-Type: application/json");
        _client.println("Content-Length: " + String(_payload.length()));
        _client.println("Authorization: Basic " + _b64.encode(_username + ":" + _password));
        _client.println();
        _client.print(_payload);

        _client.readStringUntil(' ');
        int status = _client.readStringUntil(' ').toInt();

        while (_client.connected())
        {
            String line = _client.readStringUntil('\n');
            if (line == "\r")
                break;
        }

        String response = _client.readString();

        res.code = status;
        res.response = response;

        _client.stop();
    }
    _payload = "{";
    return res;
}

zrak_client::http_response zrak_client::send(int device_id)
{

    _payload.remove(_payload.lastIndexOf(","));
    _payload += "}";

    http_response res;

    if (_https)
    {
        if (!_client_secure.connect(_host, 443))
        {
            res.code = -1;
            res.response = "Couldn't connect to host. Maybe wrong fingerprint?";
            return res;
        }

        _client_secure.println("POST " + _url_id + String(device_id) + " HTTP/1.1");
        _client_secure.println("Host: " + _host);
        _client_secure.println("User-Agent: " + _user_agent);
        _client_secure.println("Content-Type: application/json");
        _client_secure.println("Content-Length: " + String(_payload.length()));
        _client_secure.println("Authorization: Basic " + _b64.encode(_username + ":" + _password));
        _client_secure.println();
        _client_secure.print(_payload);

        _client_secure.readStringUntil(' ');
        int status = _client_secure.readStringUntil(' ').toInt();

        while (_client_secure.connected())
        {
            String line = _client_secure.readStringUntil('\n');
            if (line == "\r")
                break;
        }

        String response = _client_secure.readString();

        res.code = status;
        res.response = response;

        _client_secure.stop();
    }
    else
    {
        if (!_client.connect(_host, 80))
        {
            res.code = -1;
            res.response = "Couldn't connect to host.";
            return res;
        }

        _client.println("POST " + _url_id + String(device_id) + " HTTP/1.1");
        _client.println("Host: " + _host);
        _client.println("User-Agent: " + _user_agent);
        _client.println("Content-Type: application/json");
        _client.println("Content-Length: " + String(_payload.length()));
        _client.println("Authorization: Basic " + _b64.encode(_username + ":" + _password));
        _client.println();
        _client.print(_payload);

        _client.readStringUntil(' ');
        int status = _client.readStringUntil(' ').toInt();

        while (_client.connected())
        {
            String line = _client.readStringUntil('\n');
            if (line == "\r")
                break;
        }

        String response = _client.readString();

        res.code = status;
        res.response = response;

        _client.stop();
    }
    _payload = "{";
    return res;
}