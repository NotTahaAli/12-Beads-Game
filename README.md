# 12 Bead

## Prerequisites
### [SFML](https://www.sfml-dev.org/)
- Add SFML bin Folder in System Path (or but contents in folder with exe file.)
### [Websocketpp](https://github.com/zaphoyd/websocketpp)
### [Asio (Standalone)](https://github.com/chriskohlhoff/asio)
### [MinGW Std Threads](https://github.com/meganz/mingw-std-threads)
- Folder name as mingw-threads

## Compiling GUI (MinGW)
### Windows
```sh
del main.exe *.o
g++ -I "<sfml folder>/include" -c main.cpp
g++ -I "<Home Folder with websocketpp and mingw-threads>" -I "<asio folder>/include" -c server/socket.cpp
g++ -c server/game.cpp
g++ game.o main.o -o main -L "<sfml folder>/lib" -lsfml-graphics -lsfml-window -lsfml-system -lws2_32 -lwsock32
main
```

## Compiling consoleBased (MinGW)
### Linux
```sh
g++ consoleBased.cpp server/game.cpp -o consoleBased && ./consoleBased
```
### Windows
```bash
g++ consoleBased.cpp server/game.cpp -o consoleBased.exe && consoleBased
```