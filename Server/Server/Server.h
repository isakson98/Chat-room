

#include "stdafx.h"

struct Client_content {

	const int HEADER_LENGTH = 21;
	char hbuff[21];      // Header buffer.

	char username_buff[16];
	string username_buff_str;

	SOCKET csoc;        // Connected socket from client.
	int data_type;
	int message_length;
	int messageID;
	int nbHeaderData;   // Number of bytes read of header data.
	int verified;

	//string dbuff;
	char message[280];        // The data buffer.
	string message_str;
	int nbData;         // The number of bytes of data read.


	Client_content() {
		data_type = 0; message_length = 0; nbHeaderData = 0; nbHeaderData = 0; nbData = 0;
		memset(hbuff, 0, 9); verified = 0; messageID = 0; 
	}
};

class Server{

public: 

	Server();
	~Server();

	// win check ups
	void InitServer();
	
	//	accepts new client
	int AcceptNewClient();

	// interacts using fd set
	void InteractWclients();

	
private:

	//	activate listening socket inside InitServer()
	void ActivateListeningSoc();

	// everything to handle per client inside InteractWclient()
	bool VerifyLogin(int client_count);
	bool ReceiveMsg(int client_count);
	void SendMsg(int client_count);
	void Disconnect(int client_count);
	
	fd_set readmap;
	int Check_READMAP();

	//struct m_client_content;
	vector<Client_content> allClientData;
	vector<SOCKET> m_all_sockets;
	SOCKET m_BIG_SOC;
	SOCKET m_listening_soc;
	string m_service = "42069";
};
