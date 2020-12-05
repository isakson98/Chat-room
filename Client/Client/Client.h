#pragma once
#include "stdafx.h"

#define MESSAGE_AUTHENTICATE 0
#define MESSAGE_RECIEVE 1
#define MESSAGE_CONFIRM 2
#define MESSAGE_DENY 3
#define MESSAGE_ERROR 4
#define MESSAGE_HEADER 21
#define MESSAGE_LENGTH 280

class Client {
public:

	Client();
	~Client();

	void ClientToServer();
	void ServerToDisplay();

private:
	struct Message {
		string username;
		int type;
		int length;
		string content;
	};

	void InItClient();

	string AskForIP();

	SOCKET EstablishTCPConn(string p_host, string p_service);

	void StartUp();

	void AskForCredentials();

	bool Authenticate(string p_username, string p_password);

	void SendMsg(SOCKET p_conn, Message p_message);
	Message RecieveMsg(SOCKET p_conn);
	string ConvertToMsg(Message p_message);
	Message ParseMsg(string p_message);

	void LaunchDisplay();

	string m_username;
	string m_password;
	string m_host;
	const string m_chatService = "42069";
	const string m_displayService = "42070";

	SOCKET m_chatConn;
	SOCKET m_displayConn;
};