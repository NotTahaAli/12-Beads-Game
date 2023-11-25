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
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
using namespace rapidjson;
using namespace std;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

// Out Going Events
// 1 - Game Started
// 2 - Victory
// 3 - Loss
// 4 - Returning Game State

// Incoming Events
// 1 - Get Game State
struct gameData
{
    websocketpp::connection_hdl redPlayer, bluePlayer;
    gameState game;
};

vector<gameData> games;

websocketpp::server<websocketpp::config::asio> endpoint;

bool connectionsEqual(websocketpp::connection_hdl a, websocketpp::connection_hdl b)
{
    return a.lock().get() == b.lock().get();
}

void on_message(websocketpp::server<websocketpp::config::asio> *s, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
{
    Document eventData;
    eventData.Parse(msg->get_payload().c_str());
    if (eventData.IsObject() && eventData.HasMember("event") && eventData["event"].IsInt())
    {
        gameData game;
        for (int i = 0; i < games.size(); i++)
        {
            websocketpp::connection_hdl other;
            if (connectionsEqual(games[i].redPlayer, hdl))
                other = games[i].bluePlayer;
            else if (connectionsEqual(games[i].bluePlayer, hdl))
                other = games[i].redPlayer;
            else
                continue;
            break;
        }
        bool waitingForPartner = false;
        if (game.bluePlayer.expired())
        {
            waitingForPartner = true;
        }
        if (eventData["event"] == 1)
        {
            StringBuffer sBuff;
            Writer<StringBuffer> writer(sBuff);
            writer.StartObject();
            writer.Key("event");
            writer.Int(4);
            writer.Key("waitingForPartner");
            writer.Bool(waitingForPartner);
            if (!waitingForPartner)
            {
                writer.Key("gameOver");
                writer.Bool(game.game.gameOver);
                writer.Key("turn");
                writer.Int(game.game.turn);
                writer.Key("lastTurn");
                writer.StartObject();
                writer.Key("x");
                writer.Int(game.game.lastTurn.x);
                writer.Key("y");
                writer.Int(game.game.lastTurn.y);
                writer.EndObject();
                writer.Key("board");
                writer.StartArray();
                for (int i = 0; i < 5; i++)
                {
                    writer.StartArray();
                    for (int j = 0; j < 5; j++)
                    {
                        writer.Int(game.game.board[i][j]);
                    }
                    writer.EndArray();
                }
                writer.EndArray();
            }
            writer.EndObject();
            string outputJSON = sBuff.GetString();
            s->send(hdl, outputJSON, websocketpp::frame::opcode::text);
        }
    }
}

void on_open(websocketpp::server<websocketpp::config::asio> *s, websocketpp::connection_hdl hdl)
{
    int i = games.size();
    if (i > 0 && games[i - 1].bluePlayer.expired())
    {
        gameData &game = games[i - 1];
        game.bluePlayer = hdl;
        game.game = initGame(false);
        StringBuffer sBuff;
        Writer<StringBuffer> writer(sBuff);
        writer.StartObject();
        writer.Key("event");
        writer.Int(1);
        writer.Key("gameOver");
        writer.Bool(game.game.gameOver);
        writer.Key("turn");
        writer.Int(game.game.turn);
        writer.Key("lastTurn");
        writer.StartObject();
        writer.Key("x");
        writer.Int(game.game.lastTurn.x);
        writer.Key("y");
        writer.Int(game.game.lastTurn.y);
        writer.EndObject();
        writer.Key("board");
        writer.StartArray();
        for (int i = 0; i < 5; i++)
        {
            writer.StartArray();
            for (int j = 0; j < 5; j++)
            {
                writer.Int(game.game.board[i][j]);
            }
            writer.EndArray();
        }
        writer.EndArray();
        writer.EndObject();
        string outputJSON = sBuff.GetString();
        s->send(game.bluePlayer, outputJSON, websocketpp::frame::opcode::text);
        s->send(game.redPlayer, outputJSON, websocketpp::frame::opcode::text);
    }
    else
    {
        gameData game;
        game.redPlayer = hdl;
        games.push_back(game);
    }
}

void on_close(websocketpp::server<websocketpp::config::asio> *s, websocketpp::connection_hdl hdl)
{
    gameData game;
    for (int i = 0; i < games.size(); i++)
    {
        websocketpp::connection_hdl other;
        if (connectionsEqual(games[i].redPlayer, hdl))
            other = games[i].bluePlayer;
        else if (connectionsEqual(games[i].bluePlayer, hdl))
            other = games[i].redPlayer;
        else
            continue;
        if (!other.expired())
        {
            StringBuffer sBuff;
            Writer<StringBuffer> writer(sBuff);
            writer.StartObject();
            writer.Key("event");
            writer.Int(2);
            writer.Key("reason");
            writer.String("Other Player Left");
            writer.EndObject();
            string outputJSON = sBuff.GetString();
            endpoint.send(other, outputJSON, websocketpp::frame::opcode::text);
            s->close(other, websocketpp::close::status::normal, "Game End");
        }
        games.erase(games.begin() + i);
        break;
    }
}

const int port = 3000;

void init_server()
{
    endpoint.set_access_channels(websocketpp::log::alevel::all);
    endpoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

    endpoint.init_asio();
}

void run_server()
{
    endpoint.listen(port);
    endpoint.set_message_handler(bind(&on_message, &endpoint, ::_1, ::_2));
    endpoint.set_open_handler(bind(&on_open, &endpoint, ::_1));
    endpoint.set_close_handler(bind(&on_close, &endpoint, ::_1));
    endpoint.start_accept();
    endpoint.run();
}

int main()
{
    init_server();
    run_server();
    return 0;
}