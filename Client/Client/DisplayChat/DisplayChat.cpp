#include <iostream>
#include "DisplayChat.h"
#include "../stdafx.h"

DisplayChat::DisplayChat()
{
    std::cout << "Starting display" << std::endl;
    InIt();
    ConnectToClient();
    while (true)
    {
        Message received = ReceiveMsg(m_clientConn);
        DisplayMessage(received);
    }

}

// Listens and accepts the connection from ClientChat
void DisplayChat::ConnectToClient()
{
    cout << "Allocating listening socket" << endl;
    struct servent* pse; /* Points to service information. */
    struct sockaddr_in sin; /* Internet endpoint address. */

    /* Create an end point address for this computer. We use INADDR_ANY
    * for the address of the computer so that we may receive connection
    * requests through any of the computer's IP addresses.  */
    memset((char*)&sin, 0, sizeof(sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    /* Get the port number for the service. */
    if ((pse = getservbyname(m_clientService.c_str(), "tcp")) != NULL)
    {
        sin.sin_port = (u_short)pse->s_port;
    }
    else if ((sin.sin_port = htons((u_short)stoi(m_clientService))) == 0)
    {
        cerr << "Bad Port number/service specified: " << m_clientService << endl;;
        exit(1);
    }
    /* Allocate a socket. */
    if ((m_clientConn = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        int errorcode = WSAGetLastError();
        cerr << "socket call failed: " << errorcode << endl;
        exit(1);
    }
    /* Bind the address to the socket. */
    if (bind(m_clientConn, (struct sockaddr*)&sin, sizeof(sin)) == INVALID_SOCKET)
    {
        int errorcode = WSAGetLastError();
        cerr << "bind call failed: " << errorcode << endl;
        exit(1);
    }
    // Indicate that we are ready to wait for connects.
    if (listen(m_clientConn, 5) == INVALID_SOCKET)
    {
        int errorcode = WSAGetLastError();
        cerr << "listen call failed: " << errorcode << endl;
        exit(1);
    }
    cout << "Listening socket: " << m_clientConn << endl;

    struct sockaddr_in fsin; // Address of the client.
    int alen = sizeof(sockaddr_in);

    m_clientConn = accept(m_clientConn, (struct sockaddr*)&fsin, &alen);
    if (m_clientConn == INVALID_SOCKET) {
        int errorcode = WSAGetLastError();
        cout << "Error accepting a client " << errorcode << endl;
    }
    cout << "Connected to client" << endl;
}


DisplayChat::~DisplayChat()
{
    std::cout << "Exiting display" << std::endl;
}

void DisplayChat::DisplayMessage(Client::Message input)
{
    cout << "[" << input.username << "]: " << input.content << endl;
}

