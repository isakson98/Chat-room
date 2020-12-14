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
	void SendMsg(SOCKET p_conn, Message* p_message);
	Message ReceiveMsg(SOCKET p_conn);
	void ConvertToMsg(Message* p_message);
	Message ParseMsg(char* p_header, char* p_message, int p_length);

    SOCKET EstablishTCPConn(string p_host, string p_service);

	string m_username;
	string m_password;
	string m_host;
	const string m_chatService = "42069";
	const string m_displayService = "42070";

private:
	string AskForIP();
	void AskForCredentials();
	bool Authenticate(string p_username, string p_password);
	void LaunchDisplay();

	SOCKET m_chatConn;
	SOCKET m_displayConn;
};