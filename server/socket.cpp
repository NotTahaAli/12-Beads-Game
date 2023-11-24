#define ASIO_STANDALONE
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define _WEBSOCKETPP_MINGW_THREAD_
#endif
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>
#include <iostream>
#include <vector>
#include "../common/game.h"
using namespace std;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

struct gameData {
    int players = 0;
    websocketpp::connection_hdl redPlayer, bluePlayer;
    gameState game;
};

vector<gameData> games;

websocketpp::server<websocketpp::config::asio> endpoint;

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

void on_open(websocketpp::server<websocketpp::config::asio>* s, websocketpp::connection_hdl hdl) {
    int i = games.size();
    if (i > 0 && games[i-1].bluePlayer.expired()) {
        cout << "ABC" << endl;
        gameData &game = games[i-1];
        game.bluePlayer = hdl;
        game.game = initGame(false);
        string abc = "1 " + game.game.gameOver;
        abc.append(" " + game.game.turn);
        abc += " " + game.game.lastTurn.x;
        abc += " " + game.game.lastTurn.y;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                abc += " " + game.game.board[i][j];
            }
        }
        endpoint.send(game.bluePlayer, abc, websocketpp::frame::opcode::text);
        endpoint.send(game.redPlayer, abc, websocketpp::frame::opcode::text);
    } else {
        cout << "DEF" << endl;
        gameData game;
        game.redPlayer = hdl;
        games.push_back(game);
    }
}

void on_close(websocketpp::server<websocketpp::config::asio>* s, websocketpp::connection_hdl hdl) {

}

const int port = 3000;

void init_server() {
    endpoint.set_access_channels(websocketpp::log::alevel::all);
    endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

    endpoint.init_asio();
}

void run_server() {
    endpoint.listen(port);
    endpoint.set_message_handler(bind(&on_message,&endpoint,::_1,::_2));
    endpoint.set_open_handler(bind(&on_open, &endpoint, ::_1));
    endpoint.set_close_handler(bind(&on_close, &endpoint, ::_1));
    endpoint.start_accept();
    endpoint.run();
}

int main() {
    init_server();
    run_server();
    return 0;
}