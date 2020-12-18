

#include "stdafx.h"

#include "Server.h"



int main() {

	Server singleton;

	singleton.InitServer();

	while (true) {
		int client_pos = singleton.AcceptNewClient();
		if (client_pos < 0) return 1;
		singleton.InteractWclients();
	}
}