#define ASIO_STANDALONE
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define _WEBSOCKETPP_MINGW_THREAD_
#endif
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>
#include <iostream>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void on_message(websocketpp::server<websocketpp::config::asio>* s, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

const int port = 3000;

websocketpp::server<websocketpp::config::asio> endpoint;

void init_server() {
    endpoint.set_access_channels(websocketpp::log::alevel::all);
    endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

    endpoint.init_asio();
}

void run_server() {
    endpoint.listen(port);
    endpoint.set_message_handler(bind(&on_message,&endpoint,::_1,::_2));
    endpoint.start_accept();
    endpoint.run();
}

int main() {
    init_server();
    run_server();
    return 0;
}