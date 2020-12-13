#include <iostream>
#include "DisplayChat.h"
#include "../stdafx.h"

DisplayChat::DisplayChat()
{
    std::cout << "Starting display" << std::endl;
    InIt();
    m_clientConn = EstablishTCPConn(m_localHost, m_displayService);
    while (true)
    {
        RecieveMsg(m_clientConn);
    }
}

DisplayChat::~DisplayChat()
{
    std::cout << "Exiting display" << std::endl;
}
