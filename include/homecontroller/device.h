#pragma once

#include "homecontroller/tls_client.h"
#include "homecontroller/http_parser.h"

#include <vector>
#include <rapidjson/document.h>

namespace hc {
    class Device {
        public:
            Device();
            ~Device();

            void init();

            void login(const std::string& username, const std::string& password);
            std::string registerDevice(const std::string& name, const std::string& type);

            void run();
            virtual std::vector<uint8_t> onMessage(const std::vector<uint8_t>& msg);

            void stop();

            void setId(const std::string& deviceId) { m_deviceId = deviceId; }

        private:
            static const std::string _API_HOST;
            static const std::string _API_PORT;

            rapidjson::Document parseJSON(HTTPResponse& response);

            void sendHandshake();
            void loop();

            TLSClient m_tlsClient;
            HTTPParser m_httpParser;

            std::string m_sessionToken;
            std::string m_deviceId;

            bool m_running;
    };
}