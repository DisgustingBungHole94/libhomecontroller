#include "homecontroller/util/json_document.h"

#include "homecontroller/exception/exception.h"

#include <rapidjson/error/en.h>

namespace hc {
namespace util {

    void json_document::parse(const std::string& json_str) {
        rapidjson::ParseResult res = m_rapidjson_document.Parse(json_str.c_str());

        if (!res) {
            std::string json_err_str = rapidjson::GetParseError_En(res.Code());
            throw exception("json parse error: " + json_err_str, "hc::util::json_document::parse");
        }
    }

    std::string json_document::get_string(const std::string& name) {
        rapidjson::Value::ConstMemberIterator itr = m_rapidjson_document.FindMember(name.c_str());

        if (itr == m_rapidjson_document.MemberEnd()) {
            throw exception("member \"" + name + "\" not found", "hc::util::json_document::get_string");
        } else if (!itr->value.IsString()) {
            throw exception("member \"" + name + "\" is not a string", "hc::util::json_document::get_string");
        }

        return itr->value.GetString();
    }

    int json_document::get_int(const std::string& name) {
        rapidjson::Value::ConstMemberIterator itr = m_rapidjson_document.FindMember(name.c_str());

        if (itr == m_rapidjson_document.MemberEnd()) {
            throw exception("member \"" + name + "\" not found", "hc::util::json_document::get_int");
        } else if (!itr->value.IsInt()) {
            throw exception("member \"" + name + "\" is not an int", "hc::util::json_document::get_int");
        }

        return itr->value.GetInt();
    }

    double json_document::get_double(const std::string& name) {
        rapidjson::Value::ConstMemberIterator itr = m_rapidjson_document.FindMember(name.c_str());

        if (itr == m_rapidjson_document.MemberEnd()) {
            throw exception("member \"" + name + "\" not found", "hc::util::json_document::get_double");
        } else if (!itr->value.IsDouble()) {
            throw exception("member \"" + name + "\" is not a string", "hc::util::json_document::get_double");
        }

        return itr->value.GetDouble();
    }

    bool json_document::get_bool(const std::string& name) {
        rapidjson::Value::ConstMemberIterator itr = m_rapidjson_document.FindMember(name.c_str());

        if (itr == m_rapidjson_document.MemberEnd()) {
            throw exception("member \"" + name + "\" not found", "hc::util::json_document::get_bool");
        } else if (!itr->value.IsBool()) {
            throw exception("member \"" + name + "\" is not a string", "hc::util::json_document::get_bool");
        }

        return itr->value.GetBool();
    }

    void json_document::get_object(const std::string& name, json_document& doc_ref) {        
        rapidjson::Value::MemberIterator itr = m_rapidjson_document.FindMember(name.c_str());

        if (itr == m_rapidjson_document.MemberEnd()) {
            throw exception("member \"" + name + "\" not found", "hc::util::json_document::get_object");
        } else if (!itr->value.IsObject()) {
            throw exception("member \"" + name + "\" is not an object", "hc::util::json_document::get_object");
        }
        
        doc_ref.m_rapidjson_document.Swap(itr->value);
    }

}
}