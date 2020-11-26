#pragma once
#include "stdafx.h"

class Client {
public:

	Client();
	~Client();

private:
	struct m_Message {
		string username;
		int type;
		int length;
		string content;
	};

	void InItClient();

	void Connect();

	SOCKET EstablishTCPConn(string p_host, string p_service);

	void Authenticate();

	void StartUp();

	void LaunchDisplay();

	void Sendmsg(SOCKET p_conn);

	void RecieveMsg(SOCKET p_conn);

	m_Message ParseMsg(const char* p_message);

	string m_host;
	const string m_chatService = "42069";
	const string m_displayService = "42070";

	SOCKET m_chatConn;
	SOCKET m_displayConn;
};