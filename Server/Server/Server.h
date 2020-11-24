

#include "stdafx.h"

class Server{

public: 

	Server();
	~Server();

	// win check ups
	void InitServer();
	
	//	accepts new client
	bool AcceptNewClient();

	// one per client (as a thread)?
	void InteractWclient();

	
private:

	//	activate listening socket inside InitServer()
	void ActivateListeningSoc();

	// everything to handle per client inside InteractWclient()
	void VerifyLogin();
	void ReceiveMsg();
	void SendMsg();
	void Disconnect();

	struct m_client_header;
	SOCKET m_listening_soc;
	vector<SOCKET> m_all_client_socs;
	string m_service = "4200";
};