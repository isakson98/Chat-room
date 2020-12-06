cmake_minimum_required(VERSION 3.17)
project(Chat-room)

set(CMAKE_CXX_STANDARD 14)

add_executable(ChatServer
        Server/Server/Server.cpp
        Server/Server/Server.h
        Server/Server/Server_Main.cpp
        Server/Server/stdafx.h
        )

add_executable(ChatClient
        Client/Client/Client.cpp
        Client/Client/Client.h
        Client/Client/Client_Main.cpp
        Client/Client/stdafx.h
        )

add_executable(DisplayChat
        Client/Client/DisplayChat/DisplayChat.cpp
        Client/Client/DisplayChat/DisplayChat.h
        Client/Client/Client.h
        Client/Client/DisplayChat/DisplayChat_Main.cpp)