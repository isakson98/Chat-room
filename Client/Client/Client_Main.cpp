#include <iostream>
#include "Client.h"

using namespace std;

/*
NAME
		WaitForUserInput - Thread for sending messages
SYNOPSIS
		unsigned __stdcall WaitForUserInput(void* p_client);

		p_client -> pointer to client object
DESCRIPTION
		Due to the fact that console IO is blocking, a thread is needed to continously receive
		messages from the chatroom server. This thread uses the passed client object to 
		send user messages to the server, while the main program receives messages from the
		server and sends them to be displayed.
AUTHOR
		Milos Miladinov
*/
unsigned __stdcall WaitForUserInput(void* p_client) {
	Client* chat = (Client*)p_client;

	chat->ClientToServer();

	return 0;
}

int main() {
	Client chat;

	chat.StartUp();

	if (_beginthreadex(0, 0, WaitForUserInput, (void*)&chat, 0, 0) == 0) {
		cerr << "Error starting thread. Process aborted." << endl;
		exit(EXIT_FAILURE);
	}

	chat.ServerToDisplay();
}
