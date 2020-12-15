
// data_type
//AUTHENTICATE - 0
//MESSAGE - 1
//CONFIRM - 2
//DENY - 3
//ERROR - 4

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

	m_BIG_SOC = -2;

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
	Get_user_and_pass();
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

	m_all_sockets.push_back(m_listening_soc);
	m_BIG_SOC = m_listening_soc;

}

void Server::Get_user_and_pass() {
	ifstream infile("Username_Password.txt");

	if (infile.is_open()) {
	    cout << "file is open" << endl;
		string file_username;
		string file_password;

		while (infile >> file_username >> file_password) {
			user_pass.insert({ file_username, file_password });
		}
	} else{
	    cout << "not working" << endl;
	}
}

int Server::AcceptNewClient() {
	
	cout << "Waiting for new clients" << endl;
	
	// check if there is an impending connection
	if (Check_READMAP() < 0) {
		return -1;
	}
	// no impending connections
	if (FD_ISSET(m_listening_soc, &readmap) == false) {
		return 1;
	}

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

	if (m_BIG_SOC < newsock) {
		m_BIG_SOC = newsock;
	}
	// m_BIG_SOC = (m_BIG_SOC > newsock) ? m_BIG_SOC : newsock;

	//insert into vector of client content
	for (int isoc = 0; isoc < (int)allClientData.size(); isoc++)
	{
		// someone left, we can use his place
		if (allClientData[isoc].csoc == 0)
		{
			// bug here? fixed
			m_all_sockets[isoc + 1] = newsock;
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
	m_all_sockets.push_back(newsock);
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
void Server::InteractWclients() {

	if (Check_READMAP() < 0) {
		return;
	}

	// prioritizing new connections
	if (FD_ISSET(m_listening_soc, &readmap)) {
		return;
	}

	for (unsigned int client_num = 0; client_num < allClientData.size(); client_num++) {
		//if not set, this socket has no data currently incoming
		// index [client_num + 1] because m_all_sockets also includes listening socket
		if (!FD_ISSET(allClientData[client_num].csoc, &readmap)) { //m_all_sockets[client_num+1]
			continue;
		}
		ReceiveMsg(client_num);
		SendMsg(client_num);
	}

}

int Server::Check_READMAP() {
	// check if there is an impending connection
	FD_ZERO(&readmap);
	for (unsigned int is = 0; is < m_all_sockets.size(); is++)
	{
		if (m_all_sockets[is] == 0) continue;
		FD_SET(m_all_sockets[is], &readmap);
		m_BIG_SOC = m_BIG_SOC > m_all_sockets[is] ? m_BIG_SOC : m_all_sockets[is];
	}

	if (select(m_BIG_SOC + 1, &readmap, NULL, NULL, NULL) == SOCKET_ERROR) {
		WSACleanup();
		return -1;
	}

	return 0;
}

bool Server::ReceiveMsg(int client_count) {
	Client_content &cd = allClientData[client_count];

	// receving header data securely
	int nb;
	while (cd.nbHeaderData < cd.HEADER_LENGTH) {
		nb = recv(cd.csoc, &cd.hbuff[cd.nbHeaderData], cd.HEADER_LENGTH - cd.nbHeaderData, 0);
		if (nb <= 0)
		{
			closesocket(cd.csoc);
			Disconnect(client_count);
			cerr << "disconnect" << endl;
			return false;
		}
		cd.nbHeaderData += nb;
		if (cd.nbHeaderData == cd.HEADER_LENGTH)
		{
			cd.username_buff_str = cd.hbuff;
			cd.data_type = int(cd.hbuff[17]) - 48;
			char temp[3];
			memmove(temp, &cd.hbuff[18], 3);
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

	if (cd.verified == 0) {
		VerifyLogin(client_count);
	}

	return true;
}

// login error checking:
// 1) name too long, type is not allowed, 
bool Server::VerifyLogin(int client_count) {

	Client_content &cd = allClientData[client_count];

	// setting client to false and cd.data_type to declining authorization
	bool ClientExists = false;
	cd.hbuff[17] = '3';

	if (user_pass.find(cd.username_buff_str) != user_pass.end()) {
		if (user_pass[cd.username_buff_str] == cd.message_str) {
			ClientExists = true;
			cd.verified = 1;
			cd.data_type = 2;
			cd.hbuff[17] = '2';
		}
	}

	return ClientExists;
}

void Server::SendMsg(int client_count) {

	Client_content &cd = allClientData[client_count];

	if (cd.csoc == 0) {
		return;
	}

	//send everyone

	if (cd.data_type == 1) {
		for (unsigned int i = 0; i < allClientData.size(); ++i) {
			Client_content &client = allClientData[i];
			send(client.csoc, cd.hbuff, cd.HEADER_LENGTH, 0);
			send(client.csoc, cd.message, cd.message_length, 0);
		}
	}
	// send only to one
	else {
		send(cd.csoc, cd.hbuff, cd.HEADER_LENGTH, 0);
		send(cd.csoc, cd.message, cd.message_length, 0);
	}

	//refresh counters after 
	memset(cd.message, 0, cd.message_length);
	cd.message_str = "";
	cd.message_length = 0;
	cd.nbData = 0;
	cd.nbHeaderData = 0;
	memset(cd.hbuff, '0', cd.HEADER_LENGTH);
	return;

}

void Server::Disconnect(int client_count) {
	Client_content &cd = allClientData[client_count];
	FD_CLR(cd.csoc, &readmap);
	cd.csoc = 0;
	m_all_sockets[client_count + 1] = 0;
	cd.verified = 0;
	cd.username_buff_str = "";
	memset(cd.message, 0, cd.message_length);
	cd.message_str = "";
}