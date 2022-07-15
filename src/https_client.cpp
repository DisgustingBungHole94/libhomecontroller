#include "homecontroller/https_client.h"

#include "homecontroller/exception.h"

#include <rapidjson/error/en.h>

#include <iostream>

namespace hc {

    rapidjson::Document HTTPSResponse::parseJSON() {
        
    }

    HTTPSClient::HTTPSClient() {
        m_curl.reset(curl_easy_init());
        if (m_curl == nullptr) {
            throw Exception("failed to initialize", "HTTPSClient::HTTPSClient");
        }
    }

    HTTPSClient::~HTTPSClient() {}

    void HTTPSClient::init() {
        curl_global_init(CURL_GLOBAL_SSL);
    }

    void HTTPSClient::cleanup() {
        curl_global_cleanup();
    }

    HTTPSResponse HTTPSClient::perform(const std::string& url, RequestMethod method) {
        curl_easy_reset(m_curl.get());
        initHandle();

        CURLcode res;

        res = curl_easy_setopt(m_curl.get(), CURLOPT_URL, url.c_str());
        if (res != CURLE_OK) {
            throw Exception("failed to set url: " + std::string(m_curlErrorBuf), "HTTPSClient::perform");
        }

        switch(method) {
            case RequestMethod::GET:
                res = curl_easy_setopt(m_curl.get(), CURLOPT_HTTPGET, 1L);
                if (res != CURLE_OK) {
                    throw Exception("failed to set request as GET: " + std::string(m_curlErrorBuf), "HTTPSClient::perform");
                }
                break;
            case RequestMethod::POST:
                if (m_body == "" || m_contentType == "") {
                   throw Exception("body or content type not set", "HTTPSClient::perform");
                }

                setHeader("Content-Type", m_contentType);

                res = curl_easy_setopt(m_curl.get(), CURLOPT_POSTFIELDS, m_body.c_str());
                if (res != CURLE_OK) {
                    throw Exception("failed to set body: " + std::string(m_curlErrorBuf), "HTTPSClient::perform");
                }
                break;
        }

        res = curl_easy_perform(m_curl.get());
        if (res != CURLE_OK) {
            throw Exception("failed to perform request: " + std::string(m_curlErrorBuf), "HTTPSClient::perform");
        }

        long status;
        res = curl_easy_getinfo(m_curl.get(), CURLINFO_RESPONSE_CODE, &status);
        if (res != CURLE_OK) {
            throw Exception("failed to get status code: " + std::string(m_curlErrorBuf), "HTTPSClient::perform");
        }

        return HTTPSResponse(status, m_curlWriteBuf);
    }

    void HTTPSClient::reset() {
        m_curlWriteBuf = "";
        m_contentType = "";
        m_body = "";

        m_headers.clear();
    }

    void HTTPSClient::setHeader(const std::string& name, const std::string& value) {
        m_headers.insert(std::make_pair(name, value));
    }

    void HTTPSClient::initHandle() {
        CURLcode res;

        res = curl_easy_setopt(m_curl.get(), CURLOPT_ERRORBUFFER, m_curlErrorBuf);
        if (res != CURLE_OK) {
            throw Exception("failed to set error buffer", "HTTPSClient::initHandle");
        }

        res = curl_easy_setopt(m_curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
        if (res != CURLE_OK) {
            throw Exception("failed to redirect option: " + std::string(m_curlErrorBuf), "HTTPSClient::initHandle");
        }

        res = curl_easy_setopt(m_curl.get(), CURLOPT_WRITEFUNCTION, &HTTPSClient::writerCallback);
        if (res != CURLE_OK) {
            throw Exception("failed to set writer: " + std::string(m_curlErrorBuf), "HTTPSClient::initHandle");
        }

        res = curl_easy_setopt(m_curl.get(), CURLOPT_WRITEDATA, this);
        if (res != CURLE_OK) {
            throw Exception("failed to set write data: " + std::string(m_curlErrorBuf), "HTTPSClient::initHandle");
        }

        struct curl_slist* headers = nullptr;

        for (auto& x : m_headers) {
            std::string headerStr = x.first + ": " + x.second;
            headers = curl_slist_append(headers, headerStr.c_str());
        }

        res = curl_easy_setopt(m_curl.get(), CURLOPT_HTTPHEADER, headers);
        if (res != CURLE_OK) {
            throw Exception("failed to set header: " + std::string(m_curlErrorBuf), "HTTPSClient::initHandle");
        }
    }

    int HTTPSClient::writerCallback(char* data, size_t size, size_t nmemb, HTTPSClient* p) {
        if (p == nullptr) {
            return 0;
        }

        return p->writer(data, size, nmemb, &p->m_curlWriteBuf);
    }

    int HTTPSClient::writer(char* data, size_t size, size_t nmemb, std::string* writerData) {        
        if (writerData == nullptr) {
            return 0;
        }

        writerData->append(data, size * nmemb);

        return size * nmemb;
    }
}