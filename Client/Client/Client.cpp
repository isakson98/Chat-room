#include "Client.h"

Client::Client() {
    cout << "Hello!" << endl << endl;

    InItClient();

    Connect();

    Authenticate();

    StartUp();
}

void Client::InItClient() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {

        cerr << "Could not find useable DLL" << endl;
        exit(EXIT_FAILURE);
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        cerr << "Could not find useable DLL" << endl;
        exit(EXIT_FAILURE);
    }
}

void Client::Connect() {
    string userDecision;

    cout << "Select the applicable form of connection to the server: " << endl;
    cout << "(1) Local" << endl;
    cout << "(2) Remote" << endl;
    getline(cin, userDecision);
    cout << endl;

    if (userDecision == "1" || userDecision == "Local") {
        m_host = "127.0.0.1";
    }
    else if (userDecision == "2" || userDecision == "Remote") {
        cout << "Enter the server IP: ";
        getline(cin, m_host);
    }
    else {
        cerr << "Invalid input on user decision." << endl;
        exit(EXIT_FAILURE);
    }

    m_chatConn = EstablishTCPConn(m_host, m_chatService);
}

SOCKET Client::EstablishTCPConn(string p_host, string p_service) {
    struct hostent* he;
    struct servent* se;
    struct sockaddr_in sin;
    SOCKET s;

    memset((char*)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    if ((se = getservbyname(p_service.c_str(), "tcp")) != NULL)
    {
        sin.sin_port = se->s_port;
        printf("port No. %ld\n", (long int)ntohs(se->s_port));
    }
    else if ((sin.sin_port = htons((short int)atoi(p_service.c_str()))) == 0)
    {
        cerr << "Can't get the port number of the service." << endl;
        exit(EXIT_FAILURE);
    }

    if ((he = gethostbyname(p_host.c_str())) != NULL)
    {
        memcpy((char*)&sin.sin_addr, he->h_addr, he->h_length);
    }
    else if ((sin.sin_addr.s_addr = inet_addr(p_host.c_str())) == INADDR_NONE)
    {
        cerr << ("Invalid host address\n") << endl;
        exit(EXIT_FAILURE);
    }

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        cerr << "Could not allocate socket: " << WSAGetLastError() << endl;
        exit(EXIT_FAILURE);
    }

    if (connect(s, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        cerr << "Attempt to connect to server failed: " << WSAGetLastError() << endl;
        exit(EXIT_FAILURE);
    }

    return(s);
}

void Client::Authenticate() {
}

void Client::StartUp() {
}

Client::~Client() {
    cout << "Performing cleanup" << endl;

    cout << "Exiting client application " << endl;
    cout << "Good-bye!" << endl;
}