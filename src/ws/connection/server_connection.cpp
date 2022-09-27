#include "homecontroller/ws/connection/server_connection.h"

namespace hc {
namespace ws {
    
    void server_connection::start() {
        init();

        read_data(m_upgrade_request);
    }

}
}