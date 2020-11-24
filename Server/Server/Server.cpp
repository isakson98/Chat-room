

#include "stdafx.h"

#include "Server.h"

Server::Server() {
	cout << "Hello!" << endl;
}

Server::~Server() {
	cout << "Performing cleanup" << endl;

	cout << "Exiting server application " << endl;
	cout << "Good-bye!" << endl;
}

// activated by user
void Server::InitServer() {

	cout << endl;
	cout << "Initializing server application" << endl;
	// Make sure that we can connect:
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);  // A macro to create a word with low order and high order bytes as specified.
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0) {
		// Tell the user that we couldn't find a useable winsock.dll. */
		cerr << "Could not find useable DLL" << endl;
		exit(1);
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		// Tell the user that we couldn't find a useable winsock DLL. 
		cerr << "Could not find useable DLL" << endl;
		WSACleanup();  // Terminates the use of the library. 
		exit(1);
	}
	
	ActivateListeningSoc();
	cout << "Initialization successful" << endl;
	cout << endl;
}

// activated by user (?)
// sets up the connection of listening socket
// assigns socket value to private m_all_client_socs
void Server::ActivateListeningSoc() {

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
	if ((pse = getservbyname(m_service.c_str(), "tcp")) != NULL)
	{
		sin.sin_port = (u_short)pse->s_port;
	}
	else if ((sin.sin_port = htons((u_short)stoi(m_service))) == 0)
	{
		cerr << "Bad Port number/service specified: " << m_service << endl;;
		exit(1);
	}
	/* Allocate a socket. */
	if ((m_listening_soc = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		int errorcode = WSAGetLastError();
		cerr << "socket call failed: " << errorcode << endl;
		exit(1);
	}
	/* Bind the address to the socket. */
	if (bind(m_listening_soc, (struct sockaddr*)&sin, sizeof(sin)) == INVALID_SOCKET)
	{
		int errorcode = WSAGetLastError();
		cerr << "bind call failed: " << errorcode << endl;
		exit(1);
	}
	// Indicate that we are ready to wait for connects.
	if (listen(m_listening_soc, 5) == INVALID_SOCKET)
	{
		int errorcode = WSAGetLastError();
		cerr << "listen call failed: " << errorcode << endl;
		exit(1);
	}
	cout << "Listening socket: " << m_listening_soc << endl;

}

bool Server::AcceptNewClient() {
	
	cout << "Waiting for new clients" << endl;
	// new connection
	SOCKET newsock;
	struct sockaddr_in fsin;// Address of the client.
	int alen = sizeof(sockaddr_in);  // Length of client address.  UNIX does not 
												 // require that you specify a size.

	newsock = accept(m_listening_soc, (struct sockaddr*)&fsin, &alen);

	if (newsock == INVALID_SOCKET) {
		int errorcode = WSAGetLastError();
		cout << "Error accepting a client " << errorcode << endl;
		return false;
	}

	m_all_client_socs.push_back(newsock);
	cout << "Connection with " << inet_ntoa(fsin.sin_addr) << ", port: " << ntohs(fsin.sin_port) << endl;
	cout << endl;
	return true;
}

void Server::InteractWclient() {
		

}

void Server::VerifyLogin() {

}

void Server::ReceiveMsg() {

}

void Server::SendMsg() {

}

void Server::Disconnect() {

}