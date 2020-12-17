#ifndef CHAT_ROOM_DISPLAYCHAT_H
#define CHAT_ROOM_DISPLAYCHAT_H

#include "../Client.h"
#include "../stdafx.h"

class DisplayChat: public Client
{
public:
    DisplayChat();
    ~DisplayChat();

    void ConnectToClient();
    void DisplayMessage(Message input);
private:
    const string m_localHost = "127.0.0.1";
    const string m_clientService = "42070";
    SOCKET m_clientConn;
};


#endif //CHAT_ROOM_DISPLAYCHAT_H
