#pragma once

#include <string>
#include <memory>
#include <map>
#include <curl/curl.h>
#include <rapidjson/document.h>

namespace hc {
    namespace curl {
        template<class T> struct DeleterOf;
        template<> struct DeleterOf<CURL> { void operator()(CURL* p) const { curl_free(p); } };

        template<class CURLType>
        using UniquePtr = std::unique_ptr<CURLType, DeleterOf<CURLType>>;
    }


    

    class HTTPSClient {
        public:
            HTTPSClient();
            ~HTTPSClient();

            enum class RequestMethod {
                GET, POST
            };

            static void init();
            static void cleanup();

            HTTPSResponse perform(const std::string& url, RequestMethod method);

            void reset();

            void setBody(const std::string& contentType, const std::string& body) { 
                m_contentType = contentType;
                m_body = body;
            }
            void setHeader(const std::string& name, const std::string& value);

        private:
            void initHandle();

            static int writerCallback(char* data, size_t size, size_t nmemb, HTTPSClient* p);
            int writer(char* data, size_t size, size_t nmemb, std::string* writerData);

            curl::UniquePtr<CURL> m_curl;

            char m_curlErrorBuf[CURL_ERROR_SIZE];
            std::string m_curlWriteBuf;

            std::map<std::string, std::string> m_headers;

            std::string m_contentType;
            std::string m_body;
    };
}