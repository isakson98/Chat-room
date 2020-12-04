#include "Client.h"

Client::Client() {
    cout << "Hello! Welcome to the ChatRoom Client." << endl;
    Sleep(2000);
    cout << "Before chatting begins, we need a few things from you." << endl << endl;
    Sleep(2000);

    m_username = "";
    m_password = "";
    m_host = "";
    m_chatConn = NULL;
    m_displayConn = NULL;

    InItClient();

    m_host = AskForIP();

    m_chatConn = EstablishTCPConn(m_host, m_chatService);

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

string Client::AskForIP() {
    string userDecision;
    string host;

    cout << "Where is the server located?: " << endl;
    cout << "(1) Locally" << endl;
    cout << "(2) Remotely" << endl;

    while (true) {
        getline(cin, userDecision);
        cout << endl;

        if (userDecision == "1" || userDecision == "Locally") {
            host = "127.0.0.1";
            return host;
        }
        else if (userDecision == "2" || userDecision == "Remotely") {
            cout << "Enter the server IP: ";
            while (true) {
                getline(cin, host);
                cout << endl;

                if (inet_addr(host.c_str()) != INADDR_NONE) {
                    return host;
                }
                else {
                    cerr << "Invalid IPv4 address. Try Again: ";
                }
            }
        }
        else {
            cerr << "Invalid input on user decision. Try again: ";
        }
    }
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
    }
    else
    {
        cerr << "Can't get the port number of the service." << endl;
        exit(EXIT_FAILURE);
    }

    if ((he = gethostbyname(p_host.c_str())) != NULL)
    {
        memcpy((char*)&sin.sin_addr, he->h_addr, he->h_length);
    }
    else
    {
        cerr << ("Invalid host address") << endl;
        exit(EXIT_FAILURE);
    }

    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
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

void Client::StartUp() {
    do {
        AskForCredentials();
    } while (Authenticate(m_username, m_password) == false);

    m_displayConn = EstablishTCPConn("127.0.0.1", m_displayService);


}

void Client::AskForCredentials() {
    string username = "";
    string password = "";

    cout << "Please enter your login credentials." << endl;
    cout << "Username (Max length - 16 characters): ";

    while (true) {
        getline(cin, username);

        if (username.size() > 0 && username.size() <= 16) {
            break;
        }
        else {
            cerr << "Username exceeded max length. Try again: ";
        }
    }

    m_username = username;

    cout << "Password (Max length - 280 characters): ";

    while (true) {
        getline(cin, password);

        if (password.size() > 0 && password.size() <= 280) {
            break;
        }
        else {
            cerr << "Password exceeded max length. Try again: ";
        }
    }

    m_password = password;
}

bool Client::Authenticate(string p_username, string p_password) {
    Message login;
    login.username = m_username + "\0";
    login.type = 0;
    login.length = m_password.size();
    login.content = m_password;

    SendMsg(m_chatConn, login);

    login = RecieveMsg(m_chatConn);

    if (login.type == 2) {
        return true;
    }
    else {
        cerr << "Login credentials are invalid. Please try again." << endl << endl;
        return false;
    }
}

void Client::SendMsg(SOCKET p_conn, Message p_message) {

}

Client::Message Client::RecieveMsg(SOCKET p_conn) {

}

string Client::ConvertToMsg(Message p_message) {
    string message = "";

    message.append(p_message.username);
    message.append("\0");
    message.append(to_string(p_message.type));
    message.append(to_string(p_message.length));
    message.append(p_message.content);

    return message;
}

Client::Message Client::ParseMsg(string p_message) {
    Message message;
}

void Client::LaunchDisplay() {
    return;
}

Client::~Client() {
    cout << "Performing cleanup" << endl;

    cout << "Exiting client application " << endl;
    cout << "Good-bye!" << endl;
}