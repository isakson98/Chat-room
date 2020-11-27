

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

int Server::AcceptNewClient() {
	
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
		return -1;
	}

	//insert into vector of client content
	for (int isoc = 0; isoc < (int)allClientData.size(); isoc++)
	{
		// someone left, we can use his place
		if (allClientData[isoc].csoc == 0)
		{
			allClientData[isoc].csoc = newsock;
			allClientData[isoc].message_length = 0;
			allClientData[isoc].data_type = 0;
			allClientData[isoc].nbHeaderData = 0;
			allClientData[isoc].nbData = 0;
			return isoc;   // return index where it was at.
		}
	}
	//new client
	Client_content newClient;
	newClient.csoc = newsock;
	allClientData.push_back(newClient);

	cout << "Connection with " << inet_ntoa(fsin.sin_addr) << ", port: " << ntohs(fsin.sin_port) << endl;
	cout << endl;
	return allClientData.size() - 1;
}

// there is a private vector of structs
// one struct variable per client
// each struct has all data about the client
// client_count is for threads to know at which index
// is their particular client
void Server::InteractWclient(int client_count) {

	if (!VerifyLogin(client_count)) {
		return;
	}

	while (true) {
		ReceiveMsg(client_count);
		SendMsg(client_count);
	}
	Disconnect(client_count);
}


// login error checking:
// 1) name too long, type is not allowed, 
bool Server::VerifyLogin(int client_count) {

	Client_content &cd = allClientData[client_count];

	// dbuff will now hold password
	ReceiveMsg(client_count);

	ifstream infile("Username_Password.txt");

	bool ClientExists = false;
	cd.messageID = 3;
	cd.hbuff[16] = 3;

	if (infile.is_open()) {
		string file_username;
		string file_password;

		while (infile >> file_username >> file_password) {
			if (cd.username_buff_str == file_username) {
				if (cd.message_str == file_password) {
					ClientExists = true;
					cd.messageID = 2;
					cd.hbuff[16] = 2;
					break;
				}
			}
		}
	}

	// send message to user 
	SendMsg(client_count);

	return ClientExists;
}

bool Server::ReceiveMsg(int client_count) {
	Client_content &cd = allClientData[client_count];

	// receving header data securely
	int nb;
	while (cd.nbHeaderData < 20) {
		nb = recv(cd.csoc, &cd.hbuff[cd.nbHeaderData], 20 - cd.nbHeaderData, 0);
		if (nb <= 0)
		{
			cerr << "disconnect" << endl;
			closesocket(cd.csoc);
			cd.csoc = 0;
			return false;
		}
		cd.nbHeaderData += nb;
		if (cd.nbHeaderData == 20)
		{
			cd.username_buff_str = cd.hbuff;
			cd.data_type = int(cd.hbuff[16]) - 48;
			char temp[3];
			memmove(&temp, &cd.hbuff[17], 3);
			cd.message_length = atoi(temp);
		}
	}

	// receive message || password
	while (cd.nbData < cd.message_length)
	{
		nb = recv(cd.csoc, &cd.message[cd.nbData], cd.message_length - cd.nbData, 0);
		if (nb <= 0)
		{
			cerr << "bad disconnect" << endl;
			closesocket(cd.csoc);
			cd.csoc = 0;
			return false;
		}
		cd.nbData += nb;
	}
	cd.message_str = cd.message;
	cd.message_str = cd.message_str.substr(0, cd.message_length);

	return true;
}

void Server::SendMsg(int client_count) {

	Client_content &cd = allClientData[client_count];

	//send everyone
	if (cd.messageID == 2) {
		for (int i = 0; i < allClientData.size(); ++i) {
			Client_content &client = allClientData[i];
			send(client.csoc, client.hbuff, 20, 0);
			send(client.csoc, client.message, cd.message_length, 0);
		}
	}

	// send only to one
	else {
		send(cd.csoc, cd.hbuff, 20, 0);
	}

	//refresh counters after 
	cd.message_length = 0;
	cd.nbData = 0;
	cd.nbHeaderData = 0;

}

// is this needed?
void Server::Disconnect(int client_count) {

	Client_content &cd = allClientData[client_count];

	cd.csoc = 0;


}