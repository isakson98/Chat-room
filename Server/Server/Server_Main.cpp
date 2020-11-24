

#include "stdafx.h"

#include "Server.h"

int main() {

	Server singleton;

	singleton.InitServer();

	while (true) {
		
		if (!singleton.AcceptNewClient()) {
			return 1;
		}
		singleton.InteractWclient();
	}

}