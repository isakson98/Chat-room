#pragma once
#include "stdafx.h"

#define MESSAGE_AUTHENTICATE 0
#define MESSAGE_RECIEVE 1
#define MESSAGE_CONFIRM 2
#define MESSAGE_DENY 3
#define MESSAGE_HEADER 21
#define MESSAGE_LENGTH 280

class Client {
public:

	Client();
	~Client();

	void StartUp();
	void ClientToServer();
	void ServerToDisplay();

protected:
	struct Message {
		string username;
		int type;
		int length;
		string content;
		char message[MESSAGE_HEADER + MESSAGE_LENGTH];

		Message() {
			username = "";
			type = -1;
			length = 0;
			content = "";
			memset(message, 0, MESSAGE_HEADER + MESSAGE_LENGTH);
		}
	};

	void InIt();
	Message ReceiveMsg(SOCKET p_conn);
	Message ParseMsg(char* p_header, char* p_message, int p_length);

	string m_username;
	string m_password;
	string m_host;
	const string m_chatService = "42069";
	const string m_displayService = "42070";

private:
	string AskForIP();
	SOCKET EstablishTCPConn(string p_host, string p_service);
	void AskForCredentials();
	bool Authenticate(string p_username, string p_password);
	void SendMsg(SOCKET p_conn, Message* p_message);
	void ConvertToMsg(Message* p_message);
	void LaunchDisplay();

	SOCKET m_chatConn;
	SOCKET m_displayConn;
};