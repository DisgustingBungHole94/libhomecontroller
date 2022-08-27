#include "homecontroller/util/config.h"

#include "homecontroller/util/string.h"

#include <fstream>
#include <vector>

namespace hc {
namespace util {

    void config_value::setString(const std::string& val) {
        m_str = val;
        m_isString = true;
    }

    const std::string& config_value::getString() {
        if (!m_isString) {
            throw hc::exception("variable [" + m_name + "] is not a string.", "hc::util::config_value::getString");
        }

        return m_str;
    }

    void config_value::setInt(int val) {
        m_int = val;
        m_isInt = true;
    }

    const int config_value::getInt() {
        if (!m_isInt) {
            throw hc::exception("variable [" + m_name + "] is not an integer.", "hc::util::config_value::getInt");
        }

        return m_int;
    }

    void config::load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw hc::exception("failed to open file [" + path + "]", "hc::util::config::load");
        }

        int lineNum = 0;
        for (std::string line; std::getline(file, line); ) {
            lineNum++;

            if (line.length() < 1 || line[0] == '#') {
                continue;
            }

            std::vector<std::string> splitLine = hc::util::str::splitString(line, ' ');
            if (splitLine.size() < 2) {
                throw hc::exception("variable [" + splitLine[0] + "] is not defined (" + std::to_string(lineNum) + ")", "hc::util::config::load");
            }

            config_value val(splitLine[0]);
            int i;

            try {
                i = std::stoi(splitLine[1]);
                val.setInt(i);
            }
            catch (std::invalid_argument const& e) {
                val.setString(splitLine[1]);
            }
            catch (std::out_of_range const& e) {
                throw hc::exception("variable [" + splitLine[0] + "] is out of range (" + std::to_string(lineNum) + ")", "hc::util::config::load");
            }

            m_vars.insert(std::make_pair(splitLine[0], val));
        }
    }

    config_value& config::operator[](const std::string& var) {
        auto mit = m_vars.find(var);
        if (mit == m_vars.end()) {
            throw hc::exception("variable [" + var + "] is not defined", "hc::util::config::operator[]");
        }

        return mit->second;
    }

}
}