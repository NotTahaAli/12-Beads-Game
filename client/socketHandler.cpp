#define ASIO_STANDALONE
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define _WEBSOCKETPP_MINGW_THREAD_
#endif
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>

#include "socketHandler.h"
#include <fstream>

using namespace rapidjson;

using namespace std;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

websocketpp::client<websocketpp::config::asio_client> client;
websocketpp::lib::shared_ptr<websocketpp::lib::thread> conn_thread;
websocketpp::client<websocketpp::config::asio_client>::connection_ptr conn;

std::string URL;

bool socketClosed = true;

// Incoming Events
// 1 - Game Started
// 2 - Victory
// 3 - Loss
// 4 - Returning Game State
// 5 - Show Move
// 6 - Perform Motion
// 7 - Change Turn
// 8 - Player Number

// Outgoing Events
// 1 - Get Game State
// 2 - Get Moves
// 3 - Play Turn
// 4 - Get Player Number

void on_open(websocketpp::client<websocketpp::config::asio_client> *c, websocketpp::connection_hdl hdl)
{
    if (socketClosed)
    {
        closeSocket();
        return;
    }
    cout << "Connection Opened" << endl;
    onlineMenu.buttons[0].text.setString("Waiting For\nOther Player.");
    onlineMenu.buttons[0].text.setOrigin(onlineMenu.buttons[0].text.getGlobalBounds().getSize().x / 2.f, 2*onlineMenu.buttons[0].text.getGlobalBounds().getSize().y);
    sendGetCurrentPlayer();
}

void on_fail(websocketpp::client<websocketpp::config::asio_client> *c, websocketpp::connection_hdl hdl)
{
    socketClosed = true;
    cout << "Connection Failed" << endl;
    onlineMenu.buttons[0].text.setString("Connection Failed.");
    onlineMenu.buttons[0].text.setOrigin(onlineMenu.buttons[0].text.getGlobalBounds().getSize().x / 2.f, 3.5*onlineMenu.buttons[0].text.getGlobalBounds().getSize().y);
}

void on_close(websocketpp::client<websocketpp::config::asio_client> *c, websocketpp::connection_hdl hdl)
{
    cout << "Connection Closed" << endl;
    if (!socketClosed && board.visible && !board.blocked)
    {
        showPopup("Connection Closed");
    } else if (onlineMenu.visible) {
    onlineMenu.buttons[0].text.setString("Connection Closed.");
    onlineMenu.buttons[0].text.setOrigin(onlineMenu.buttons[0].text.getGlobalBounds().getSize().x / 2.f, 3.5*onlineMenu.buttons[0].text.getGlobalBounds().getSize().y);
    }
    socketClosed = true;
}

void on_message(websocketpp::client<websocketpp::config::asio_client> *c, websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg)
{
    cout << "Message Recieved " << msg->get_payload() << endl;
    Document eventData;
    eventData.Parse(msg->get_payload().c_str());
    if (eventData.IsObject() && eventData.HasMember("event") && eventData["event"].IsInt())
    {
        if (eventData["event"] == 8)
        {
            if (eventData.HasMember("player") && eventData["player"].IsInt())
            {
                cout << currentPlayer << endl;
                currentPlayer = eventData["player"].GetInt();
                cout << currentPlayer << endl;
            }
        }
        else if (eventData["event"] == 7)
        {
            resetHighlights(board);
            if (eventData.HasMember("turn") && eventData["turn"].IsInt())
                board.game.turn = eventData["turn"].GetInt();
            if (eventData.HasMember("lastTurn") && eventData["lastTurn"].IsObject() && eventData["lastTurn"].HasMember("x") && eventData["lastTurn"].HasMember("y") && eventData["lastTurn"]["x"].IsInt() && eventData["lastTurn"]["y"].IsInt())
                board.game.lastTurn = {eventData["lastTurn"]["x"].GetInt(), eventData["lastTurn"]["y"].GetInt()};
        }
        else if (eventData["event"] == 6)
        {
            if (eventData.HasMember("from") && eventData["from"].IsObject() && eventData["from"].HasMember("x") && eventData["from"].HasMember("y") && eventData["from"]["x"].IsInt() && eventData["from"]["y"].IsInt() && eventData.HasMember("to") && eventData["to"].IsObject() && eventData["to"].HasMember("x") && eventData["to"].HasMember("y") && eventData["to"]["x"].IsInt() && eventData["to"]["y"].IsInt())
            {
                if (eventData["to"]["x"] == -1)
                {
                    removeBead(board.beads[eventData["from"]["x"].GetInt()][eventData["from"]["y"].GetInt()]);
                    board.game.board[eventData["from"]["x"].GetInt()][eventData["from"]["y"].GetInt()] = 0;
                }
                else
                {
                    moveBead(board, board.beads[eventData["from"]["x"].GetInt()][eventData["from"]["y"].GetInt()], {eventData["to"]["x"].GetInt(), eventData["to"]["y"].GetInt()});
                    board.game.board[eventData["to"]["x"].GetInt()][eventData["to"]["y"].GetInt()] = board.game.board[eventData["from"]["x"].GetInt()][eventData["from"]["y"].GetInt()];
                    board.game.board[eventData["from"]["x"].GetInt()][eventData["from"]["y"].GetInt()] = 0;
                }
            }
        }
        else if (eventData["event"] == 5)
        {
            if (eventData.HasMember("first") && eventData["first"].IsBool() && eventData["first"] == true)
                resetHighlights(board);
            if (eventData.HasMember("pos") && eventData["pos"].IsObject() && eventData["pos"].HasMember("x") && eventData["pos"].HasMember("y") && eventData["pos"]["x"].IsInt() && eventData["pos"]["y"].IsInt() && eventData.HasMember("move") && eventData["move"].IsObject() && eventData["move"].HasMember("i") && eventData["move"].HasMember("j") && eventData["move"].HasMember("val") && eventData["move"]["i"].IsInt() && eventData["move"]["j"].IsInt() && eventData["move"]["val"].IsInt())
            {
                showHighlight(board, {eventData["pos"]["x"].GetInt(), eventData["pos"]["y"].GetInt()}, {eventData["move"]["i"].GetInt(), eventData["move"]["j"].GetInt()}, eventData["move"]["val"].GetInt());
            }
        }
        else if (eventData["event"] == 3)
        {
            board.game.gameOver = true;
            board.game.turn = 0;
            if (backgroundMusic.getStatus() == sf::Music::Status::Playing) {
                defeatSound.play();
            }
            showPopup("You Lost!");
        }
        else if (eventData["event"] == 2)
        {
            board.game.gameOver = true;
            board.game.turn = 0;
            if (backgroundMusic.getStatus() == sf::Music::Status::Playing) {
                victorySound.play();
            }
            showPopup("You Won!");
        }
        else if (eventData["event"] == 1)
        {
            startNewPassAndPlayGame();
            board.isOnline = true;
            if (eventData.HasMember("gameOver") && eventData["gameOver"].IsBool())
                board.game.gameOver = eventData["gameOver"].GetBool();
            if (eventData.HasMember("turn") && eventData["turn"].IsInt())
                board.game.turn = eventData["turn"].GetInt();
            if (eventData.HasMember("lastTurn") && eventData["lastTurn"].IsObject() && eventData["lastTurn"].HasMember("x") && eventData["lastTurn"].HasMember("y") && eventData["lastTurn"]["x"].IsInt() && eventData["lastTurn"]["y"].IsInt())
                board.game.lastTurn = {eventData["lastTurn"]["x"].GetInt(), eventData["lastTurn"]["y"].GetInt()};
            if (eventData.HasMember("board") && eventData["board"].IsArray() && eventData["board"].GetArray().Size() == 5 && eventData["board"][0].GetArray().Size() == 5)
            {
                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        board.game.board[i][j] = eventData["board"][i][j].GetInt();
                    }
                }
            }
            board.game.saveable = false;
        }
    }
}

void setupClient()
{
    ifstream configFile("./data/config.json");
    Document configData;
    if (configFile.is_open()) {
        IStreamWrapper isw(configFile);
        configData.ParseStream(isw);
    }
    configFile.close();
    configFile.close();
    if (configData.IsObject() && configData.HasMember("url") && configData["url"].IsString()) {
        URL = configData["url"].GetString();
    } else {
        URL = "ws://localhost:3000";
        ofstream configFile("./data/config.json");
        OStreamWrapper osw(configFile);
        Writer<OStreamWrapper> writer(osw);
        writer.StartObject();
        writer.Key("url");
        writer.String(URL.c_str());
        writer.EndObject();
        configFile.close();
    }

    client.clear_access_channels(websocketpp::log::alevel::all);
    client.clear_error_channels(websocketpp::log::elevel::all);
    client.init_asio();
    client.start_perpetual();
    conn_thread.reset(new websocketpp::lib::thread(&websocketpp::client<websocketpp::config::asio_client>::run, &client));

    client.set_open_handler(bind(&on_open, &client, ::_1));
    client.set_fail_handler(bind(&on_fail, &client, ::_1));
    client.set_close_handler(bind(&on_close, &client, ::_1));
    client.set_message_handler(bind(&on_message, &client, ::_1, ::_2));
}

void closeSocket()
{
    if (conn && conn->get_state() == websocketpp::session::state::open)
    {
        client.close(conn->get_handle(), websocketpp::close::status::going_away, "");
    }
    else
    {
        socketClosed = true;
    }
}

void closeClient()
{
    client.stop_perpetual();
    closeSocket();
    conn_thread->join();
}

bool connect(std::string const URL)
{
    websocketpp::lib::error_code ec;
    socketClosed = false;
    conn = client.get_connection(URL, ec);
    if (ec)
    {
        cout << "Error Connection to " << URL << " : " << ec.message() << endl;
        return false;
    }

    client.connect(conn);
    return true;
}

void sendGetGameState()
{
    StringBuffer sBuff;
    Writer<StringBuffer> writer(sBuff);
    writer.StartObject();
    writer.Key("event");
    writer.Int(1);
    writer.EndObject();
    client.send(conn->get_handle(), sBuff.GetString(), websocketpp::frame::opcode::text);
}

void sendGetMoves(sf::Vector2i gridPos)
{
    StringBuffer sBuff;
    Writer<StringBuffer> writer(sBuff);
    writer.StartObject();
    writer.Key("event");
    writer.Int(2);
    writer.Key("pos");
    writer.StartObject();
    writer.Key("x");
    writer.Int(gridPos.x);
    writer.Key("y");
    writer.Int(gridPos.y);
    writer.EndObject();
    writer.EndObject();
    client.send(conn->get_handle(), sBuff.GetString(), websocketpp::frame::opcode::text);
}

void sendPlayMove(sf::Vector2i gridPos, sf::Vector2i move)
{
    StringBuffer sBuff;
    Writer<StringBuffer> writer(sBuff);
    writer.StartObject();
    writer.Key("event");
    writer.Int(3);
    writer.Key("pos");
    writer.StartObject();
    writer.Key("x");
    writer.Int(gridPos.x);
    writer.Key("y");
    writer.Int(gridPos.y);
    writer.EndObject();
    writer.Key("move");
    writer.StartObject();
    writer.Key("x");
    writer.Int(move.x);
    writer.Key("y");
    writer.Int(move.y);
    writer.EndObject();
    writer.EndObject();
    client.send(conn->get_handle(), sBuff.GetString(), websocketpp::frame::opcode::text);
}

void sendGetCurrentPlayer()
{
    StringBuffer sBuff;
    Writer<StringBuffer> writer(sBuff);
    writer.StartObject();
    writer.Key("event");
    writer.Int(4);
    writer.EndObject();
    client.send(conn->get_handle(), sBuff.GetString(), websocketpp::frame::opcode::text);
}

// int main()
// {
//     setupClient();
//     char a;
//     connect("ws://localhost:3000");
//     while (true)
//     {
//         cin >> a;
//         if (a == 'x')
//             break;
//         if (a == 'c')
//         {
//             StringBuffer sBuff;
//             Writer<StringBuffer> writer(sBuff);
//             writer.StartObject();
//             writer.Key("event");
//             writer.Int(4);
//             writer.EndObject();
//             client.send(conn->get_handle(), sBuff.GetString(), websocketpp::frame::opcode::text);
//         }
//     }
//     closeClient();
//     return 0;
// }