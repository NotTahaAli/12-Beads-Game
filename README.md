# 12 Bead
![Static Badge](https://img.shields.io/badge/Download-Windows_64_Bit-blue?style=for-the-badge&link=https%3A%2F%2Fgithub.com%2Fm-tahaali%2F12-Beads-Game%2Freleases%2Ftag%2Fv1.0.0)

## Prerequisites for Compiling
### [SFML](https://www.sfml-dev.org/)
- Available in package managers
- Add SFML bin Folder in System Path (or but contents in folder with exe file.) (For Windows Only)
### [Websocketpp](https://github.com/zaphoyd/websocketpp)
- Available in package managers
### [Asio (Standalone)](https://github.com/chriskohlhoff/asio)
- Available in package managers
### [MinGW Std Threads (Windows Only)](https://github.com/meganz/mingw-std-threads)
- Folder name as mingw-threads
### [RapidJSON](https://rapidjson.org/index.html)

## Compiling GUI (MinGW)
### Windows (Static Linking)
```sh
del main.exe *.o *.res
windres icon.rc -O coff icon.res
g++ -I "<sfml folder>/include" -c client/gui.cpp -c client/gameHandler.cpp -c client/menuHandler.cpp -D SFML_STATIC
g++ -I "<sfml folder>/include" -I "<Home Folder with websocketpp and mingw-threads>" -I "<asio folder>/include" -I "<rapidjson folder>/include" -c client/socketHandler.cpp -D SFML_STATIC
g++ -c common/game.cpp client/AI.cpp
g++ game.o AI.cpp gui.o gameHandler.o menuHandler.o socketHandler.o icon.res -o main -static -L "<sfml folder>/lib" -lsfml-audio-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg -lopengl32 -lfreetype -lwinmm -lgdi32 -lwsock32 -lws2_32
./main
```
### Windows (Dynamic Linking)
```sh
del main.exe *.o *.res
windres icon.rc -O coff icon.res
g++ -I "<sfml folder>/include" -c client/gui.cpp -c client/gameHandler.cpp -c client/menuHandler.cpp
g++ -I "<sfml folder>/include" -I "<Home Folder with websocketpp and mingw-threads>" -I "<asio folder>/include" -I "<rapidjson folder>/include" -c client/socketHandler.cpp
g++ -c common/game.cpp
g++ game.o gui.o gameHandler.o menuHandler.o socketHandler.o icon.res -o main -L "<sfml folder>/lib" -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -lwsock32 -lws2_32
./main
```
### Linux
```sh
g++ common/game.cpp client/gui.cpp client/gameHandler.cpp client/menuHandler.cpp client/socketHandler.cpp -o main -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system
./main
```

## Compiling Server (MinGW)
### Windows
```sh
del server.exe *.o
g++ -I "<Home Folder with websocketpp and mingw-threads>" -I "<asio folder>/include" -I "<rapidjson folder>/include" -c server/socket.cpp
g++ -c common/game.cpp
g++ socket.o game.o -o server -lws2_32 -lwsock32
```

### Linux
```sh
g++ server/socket.cpp common/game.cpp -o Server
./Server
```

## Compiling consoleBased (MinGW)
```sh
g++ consoleBased.cpp common/game.cpp client/AI.cpp -o consoleBased
./consoleBased
```
