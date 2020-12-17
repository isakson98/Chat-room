

#include "stdafx.h"

/*

Purpose:
one struct per client. Data is cleaned after a disconnection
it saves all data for the existence of the client

*/


struct Client_content {

	int HEADER_LENGTH = 21;
	char hbuff[21];      // Header buffer.

	string username_buff_str;

	SOCKET csoc;        // Connected socket from client.
	int message_length;
	int nbHeaderData;   // Number of bytes read of header data.
	int verified;

	char message[280];  // The data buffer.
	string message_str;
	int nbData;         // The number of bytes of data read.


	Client_content() {
		message_length = 0; nbHeaderData = 0; nbHeaderData = 0; nbData = 0;
		memset(hbuff, 0, 9); verified = 0; 
	}
};


/*

Purpose:
One Server object performs all the duties for a server.
The user is given a few simple functions to implement
while the workload is split among smaller functions "under the hood"

*/

class Server{

public: 

	Server();
	~Server();

	// functions used by user
	void InitServer();
	int AcceptNewClient();
	void InteractWclients();

	
private:

	//	activate listening socket inside InitServer()
	void ActivateListeningSoc();

	// get all usernames and passwords for faster access
	void Get_user_and_pass();
	unordered_map <string, string> user_pass;

	// everything to handle per client inside InteractWclient()
	bool VerifyLogin(int client_count);
	bool ReceiveMsg(int client_count);
	void SendMsg(int client_count);
	void Disconnect(int client_count);
	
	fd_set readmap;
	int Check_READMAP();

	vector<Client_content> allClientData;
	vector<SOCKET> m_all_sockets;
	SOCKET m_BIG_SOC;
	SOCKET m_listening_soc;
	string m_service = "42069";
};
