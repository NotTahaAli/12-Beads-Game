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

// T-ALI
const int port = 8254;

// Out Going Events
// 1 - Game Started
// 2 - Victory
// 3 - Loss
// 4 - Returning Game State
// 5 - Show Move
// 6 - Perform Motion
// 7 - Change Turn
// 8 - Player Number

// Incoming Events
// 1 - Get Game State
// 2 - Get Moves
// 3 - Play Turn
// 4 - Get Player Number
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

void ShowMoves(websocketpp::server<websocketpp::config::asio> *s, websocketpp::connection_hdl hdl, gameData game, Coordinates pos)
{
    int moves[3][3];
    StringBuffer sBuff;
    Writer<StringBuffer> writer(sBuff);
    getMoves(game.game, pos, moves);
    bool first = true;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (moves[i][j] == 0)
                continue;
            sBuff.Clear();
            writer.Reset(sBuff);
            writer.StartObject();
            writer.Key("event");
            writer.Int(5);
            writer.Key("first");
            writer.Bool(first);
            writer.Key("pos");
            writer.StartObject();
            writer.Key("x");
            writer.Int(pos.x);
            writer.Key("y");
            writer.Int(pos.y);
            writer.EndObject();
            writer.Key("move");
            writer.StartObject();
            writer.Key("i");
            writer.Int(i);
            writer.Key("j");
            writer.Int(j);
            writer.Key("val");
            writer.Int(moves[i][j]);
            writer.EndObject();
            writer.EndObject();
            first = false;
            string outputJSON = sBuff.GetString();
            s->send(hdl, outputJSON, websocketpp::frame::opcode::text);
        }
    }
}

void sendMotion(websocketpp::server<websocketpp::config::asio> *s, gameData game, Motion motion)
{
    StringBuffer sBuff;
    Writer<StringBuffer> writer(sBuff);
    writer.StartObject();
    writer.Key("event");
    writer.Int(6);
    writer.Key("from");
    writer.StartObject();
    writer.Key("x");
    writer.Int(motion.from.x);
    writer.Key("y");
    writer.Int(motion.from.y);
    writer.EndObject();
    writer.Key("to");
    writer.StartObject();
    writer.Key("x");
    writer.Int(motion.to.x);
    writer.Key("y");
    writer.Int(motion.to.y);
    writer.EndObject();
    writer.EndObject();
    string outputJSON = sBuff.GetString();
    s->send(game.bluePlayer, outputJSON, websocketpp::frame::opcode::text);
    s->send(game.redPlayer, outputJSON, websocketpp::frame::opcode::text);
}

void on_message(websocketpp::server<websocketpp::config::asio> *s, websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
{
    Document eventData;
    eventData.Parse(msg->get_payload().c_str());
    if (eventData.IsObject() && eventData.HasMember("event") && eventData["event"].IsInt())
    {
        int currentPlayer;
        websocketpp::connection_hdl other;
        int i = 0;
        for (i = 0; i < games.size(); i++)
        {
            if (connectionsEqual(games[i].redPlayer, hdl))
            {
                other = games[i].bluePlayer;
                currentPlayer = 1;
            }
            else if (connectionsEqual(games[i].bluePlayer, hdl))
            {
                other = games[i].redPlayer;
                currentPlayer = -1;
            }
            else
                continue;
            break;
        }
        gameData &game = games[i];
        bool waitingForPartner = false;
        if (game.game.gameOver)
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
        else if (eventData["event"] == 2)
        {
            if (!eventData.HasMember("pos") || !eventData["pos"].IsObject())
                return;
            if (!eventData["pos"].HasMember("x") || !eventData["pos"]["x"].IsInt())
                return;
            if (!eventData["pos"].HasMember("y") || !eventData["pos"]["y"].IsInt())
                return;
            if (currentPlayer == game.game.turn)
            {
                ShowMoves(s, hdl, game, {eventData["pos"]["x"].GetInt(), eventData["pos"]["y"].GetInt()});
            }
        }
        else if (eventData["event"] == 3)
        {
            if (!eventData.HasMember("pos") || !eventData["pos"].IsObject())
                return;
            if (!eventData["pos"].HasMember("x") || !eventData["pos"]["x"].IsInt())
                return;
            if (!eventData["pos"].HasMember("y") || !eventData["pos"]["y"].IsInt())
                return;
            if (!eventData.HasMember("move") || !eventData["move"].IsObject())
                return;
            if (!eventData["move"].HasMember("x") || !eventData["move"]["x"].IsInt())
                return;
            if (!eventData["move"].HasMember("y") || !eventData["move"]["y"].IsInt())
                return;
            if (currentPlayer != game.game.turn)
                return;
            turnData turndata = playTurn(game.game, {eventData["pos"]["x"].GetInt(), eventData["pos"]["y"].GetInt()}, {eventData["move"]["x"].GetInt(), eventData["move"]["y"].GetInt()});
            if (turndata.status != 0)
            {
                sendMotion(s, game, turndata.move);
                if (turndata.remove.from.x != -1)
                {
                    sendMotion(s, game, turndata.remove);
                }

                if (turndata.status == 1)
                {
                    StringBuffer sBuff;
                    Writer<StringBuffer> writer(sBuff);
                    writer.StartObject();
                    writer.Key("event");
                    writer.Int(7);
                    writer.Key("lastTurn");
                    writer.StartObject();
                    writer.Key("x");
                    writer.Int(game.game.lastTurn.x);
                    writer.Key("y");
                    writer.Int(game.game.lastTurn.y);
                    writer.EndObject();
                    writer.EndObject();
                    string outputJSON = sBuff.GetString();
                    s->send(hdl, outputJSON, websocketpp::frame::opcode::text);
                    cout << "AAAAAHAHAHAHAA ";
                    cout << turndata.move.to.x << " ";
                    cout << turndata.move.to.y << endl;
                    ShowMoves(s, hdl, game, turndata.move.to);
                }
                else if (turndata.status == 3)
                {
                    int winner = checkVictory(game.game);
                    StringBuffer vBuff;
                    StringBuffer lBuff;
                    Writer<StringBuffer> writer(vBuff);
                    writer.StartObject();
                    writer.Key("event");
                    writer.Int(2);
                    writer.EndObject();
                    writer.Reset(lBuff);
                    writer.StartObject();
                    writer.Key("event");
                    writer.Int(3);
                    writer.EndObject();
                    if (winner == 1)
                    {
                        s->send(game.redPlayer, vBuff.GetString(), websocketpp::frame::opcode::text);
                        s->send(game.bluePlayer, lBuff.GetString(), websocketpp::frame::opcode::text);
                    }
                    else
                    {
                        s->send(game.bluePlayer, vBuff.GetString(), websocketpp::frame::opcode::text);
                        s->send(game.redPlayer, lBuff.GetString(), websocketpp::frame::opcode::text);
                    }
                    s->close(game.redPlayer, websocketpp::close::status::normal, "Game End");
                    s->close(game.bluePlayer, websocketpp::close::status::normal, "Game End");
                    games.erase(games.begin() + i);
                }
                else
                {
                    StringBuffer sBuff;
                    Writer<StringBuffer> writer(sBuff);
                    writer.StartObject();
                    writer.Key("event");
                    writer.Int(7);
                    writer.Key("turn");
                    writer.Int(game.game.turn);
                    writer.Key("lastTurn");
                    writer.StartObject();
                    writer.Key("x");
                    writer.Int(game.game.lastTurn.x);
                    writer.Key("y");
                    writer.Int(game.game.lastTurn.y);
                    writer.EndObject();
                    writer.EndObject();
                    string outputJSON = sBuff.GetString();
                    s->send(game.bluePlayer, outputJSON, websocketpp::frame::opcode::text);
                    s->send(game.redPlayer, outputJSON, websocketpp::frame::opcode::text);
                }
            }
        }
        else if (eventData["event"] == 4)
        {
            StringBuffer sBuff;
            Writer<StringBuffer> writer(sBuff);
            writer.StartObject();
            writer.Key("event");
            writer.Int(8);
            writer.Key("player");
            writer.Int(currentPlayer);
            writer.EndObject();
            s->send(hdl, sBuff.GetString(), websocketpp::frame::opcode::text);
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
    for (int i = 0; i < games.size(); i++)
    {
        websocketpp::connection_hdl other;
        if (connectionsEqual(games[i].redPlayer, hdl))
            other = games[i].bluePlayer;
        else if (connectionsEqual(games[i].bluePlayer, hdl))
            other = games[i].redPlayer;
        else
            continue;
        if (!games[i].game.gameOver && !other.expired())
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
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        system("ipconfig");
    #else
        system('echo "IP Address:" && hostname -I | awk "{print $1}"');
    #endif
    cout << "Port: " << port << endl;
    run_server();
    return 0;
}