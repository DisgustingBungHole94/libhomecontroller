#include "homecontroller/app/device.h"

/*#include "homecontroller/exception.h"

#include <rapidjson/error/en.h>
#include <iostream>
#include <algorithm>

namespace hc {
    const std::string Device::_API_HOST = "api.bunghole.lol";
    const std::string Device::_API_PORT = "4434"; 

    Device::Device() 
        : m_sessionToken(""), m_deviceId(""), m_running(false)
    {}

    Device::~Device() {}

    void Device::init() {
        m_tlsClient.init();
        m_httpParser.init();

        m_tlsClient.connect(_API_HOST, _API_PORT);
    }

    void Device::login(const std::string& username, const std::string& password) {
        std::string json = "{";
        json += "\"username\":\"" + username + "\",";
        json += "\"password\":\"" + password + "\"";
        json += "}";

        std::string request = "POST /login HTTP/1.1\r\n";
        request += "Host: " + _API_HOST + ":" + _API_PORT + "\r\n";
        request += "Connection: keep-alive\r\n";
        request += "Content-Type: application/json\r\n";
        request += "Content-Length: " + std::to_string(json.length()) + "\r\n";
        request += "\r\n";
        request += json;

        m_tlsClient.send(request);
        std::string response = m_tlsClient.recv();

        HTTPResponse res = m_httpParser.parse(response);

        rapidjson::Document document = parseJSON(res);

        if (!(document.HasMember("token") && document["token"].IsString())) {
            throw Exception("invalid JSON response", "Device::login");
        }

        m_sessionToken = document["token"].GetString();
    }

    std::string Device::registerDevice(const std::string& name, const std::string& type) {
        if (m_sessionToken == "") throw Exception("not logged in", "Device::registerDevice");

        std::string json = "{";
        json += "\"device_name\":\"" + name + "\",";
        json += "\"device_type\":\"" + type + "\"";
        json += "}";

        std::string request = "POST /register HTTP/1.1\r\n";
        request += "Host: " + _API_HOST + ":" + _API_PORT + "\r\n";
        request += "Connection: keep-alive\r\n";
        request += "Content-Type: application/json\r\n";
        request += "Content-Length: " + std::to_string(json.length()) + "\r\n";
        request += "Authorization: HomeController " + m_sessionToken + "\r\n";
        request += "\r\n";
        request += json;

        m_tlsClient.send(request);
        std::string response = m_tlsClient.recv();

        HTTPResponse res = m_httpParser.parse(response);

        rapidjson::Document document = parseJSON(res);
        
        if (!(document.HasMember("device_id") && document["device_id"].IsString())) {
            throw Exception("invalid JSON response", "Device::registerDevice");
        }

        return document["device_id"].GetString();
    }

    void Device::run() {
        if (m_sessionToken == "") throw Exception("not logged in", "Device::registerDevice");
        if (m_deviceId == "") throw Exception("device id not set", "Device::registerDevice");

        std::string request = "GET / HTTP/1.1\r\n";
        request += "Host: " + _API_HOST + ":" + _API_PORT + "\r\n";
        request += "Connection: Upgrade\r\n";
        request += "Upgrade: hcdevice\r\n";
        request += "\r\n";

        m_tlsClient.send(request);
        std::string response = m_tlsClient.recv();

        HTTPResponse res = m_httpParser.parse(response);

        if (!res.shouldUpgrade()) {
            throw Exception("connection upgrade failed", "Device::run");
        }

        sendHandshake();
        loop();   
    }

    std::vector<uint8_t> Device::onMessage(const std::vector<uint8_t>& msg) {
        return std::vector<uint8_t>(0x00);
    }

    void Device::stop() {
        m_tlsClient.stop();
    }

    void Device::sendHandshake() {
        std::string handshakeMsg = m_sessionToken + m_deviceId;
        std::string handshakeRes;

        m_tlsClient.send(handshakeMsg);
        handshakeRes = m_tlsClient.recv();

        if (handshakeRes.size() < 1) {
            throw Exception("invalid handshake response", "Device::run");
        } else if (handshakeRes[0] != 0x00) {
            throw Exception("handshake failed: " + std::to_string(handshakeRes[0]), "Device::run");
        }

        m_running = true;
    }

    void Device::loop() {
        while(m_running) {
            std::string msg = m_tlsClient.recv();

            if (msg.size() == 1 && msg[0] == 0x00) {
                std::string pong;
                pong.resize(1);
                pong.at(0) = 0x00;

                m_tlsClient.send(pong);
            } else {
                if (msg.size() < 5) {
                    std::string res;
                    res.resize(1);
                    res.at(0) = 0xFE;

                    m_tlsClient.send(res);
                    continue;
                }

                std::string res(msg.begin(), msg.begin() + 4);

                std::vector<uint8_t> msgBytes(msg.begin()+ 4, msg.end());

                std::vector<uint8_t> resBytes = onMessage(msgBytes);
                res.append(resBytes.begin(), resBytes.end());
                
                m_tlsClient.send(std::string(res.begin(), res.end()));
            }
        }
    }

    rapidjson::Document Device::parseJSON(HTTPResponse& response) {
        rapidjson::Document document = response.parseJSON();

        if (!(document.HasMember("success") && document["success"].IsBool())) {
            throw Exception("invalid JSON response", "Device::parseJSON");
        }

        if (!document["success"].GetBool() || response.getStatus() != "OK") {
            if (document.HasMember("error_msg") && document["error_msg"].IsString()) {
                throw Exception("request failed: " + std::string(document["error_msg"].GetString()), "Device::parseJSON");
            } else {
                throw Exception("request failed", "Device::parseJSON");
            }
        }

        return document;
    }
}*/