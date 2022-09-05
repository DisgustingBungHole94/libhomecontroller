#include "homecontroller/util/config.h"

#include "homecontroller/util/string.h"

#include <fstream>
#include <vector>

namespace hc {
namespace util {

    void config_value::set_string(const std::string& val) {
        m_str = val;
        m_is_string = true;
    }

    const std::string& config_value::get_string() {
        if (!m_is_string) {
            throw exception("variable [" + m_name + "] is not a string.", "hc::util::config_value::getString");
        }

        return m_str;
    }

    void config_value::set_int(int val) {
        m_int = val;
        m_is_int = true;
    }

    const int config_value::get_int() {
        if (!m_is_int) {
            throw exception("variable [" + m_name + "] is not an integer.", "hc::util::config_value::getInt");
        }

        return m_int;
    }

    void config::load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw exception("failed to open file [" + path + "]", "hc::util::config::load");
        }

        int line_num = 0;
        for (std::string line; std::getline(file, line); ) {
            line_num++;

            if (line.length() < 1 || line[0] == '#') {
                continue;
            }

            std::vector<std::string> split_line = str::split_string(line, ' ');
            if (split_line.size() < 2) {
                throw exception("variable [" + split_line[0] + "] is not defined (" + std::to_string(line_num) + ")", "hc::util::config::load");
            }

            config_value val(split_line[0]);
            int i;

            try {
                i = std::stoi(split_line[1]);
                val.set_int(i);
            }
            catch (std::invalid_argument const& e) {
                val.set_string(split_line[1]);
            }
            catch (std::out_of_range const& e) {
                throw exception("variable [" + split_line[0] + "] is out of range (" + std::to_string(line_num) + ")", "hc::util::config::load");
            }

            m_vars.insert(std::make_pair(split_line[0], val));
        }
    }

    config_value& config::operator[](const std::string& var) {
        auto mit = m_vars.find(var);
        if (mit == m_vars.end()) {
            throw exception("variable [" + var + "] is not defined", "hc::util::config::operator[]");
        }

        return mit->second;
    }

}
}