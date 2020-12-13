#include <iostream>
#include "Client.h"

using namespace std;

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
