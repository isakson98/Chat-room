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
    struct sockaddr_in sin;
    SOCKET s;

    memset((char*)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    if ((sin.sin_port = htons((short int)atoi(p_service.c_str())))== 0)
    {
        cerr << "Can't get the port number of the service." << endl;
        exit(EXIT_FAILURE);
    }

    if ((sin.sin_addr.s_addr = inet_addr(p_host.c_str())) == INADDR_NONE)
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

    //m_displayConn = EstablishTCPConn("127.0.0.1", m_displayService);

    //LaunchDisplay();
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
            cerr << "Username empty or exceeded max length. Try again: ";
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
            cerr << "Password empty or exceeded max length. Try again: ";
        }
    }

    m_password = password;
}

bool Client::Authenticate(string p_username, string p_password) {
    Message login;
    login.username = m_username + '\0';
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
    if (send(p_conn, ConvertToMsg(p_message).c_str(), p_message.length + MESSAGE_HEADER, 0) == SOCKET_ERROR) {
        cerr << "Send returned an error with error code: " << WSAGetLastError() << endl;
        closesocket(p_conn);
        exit(EXIT_FAILURE);
    }
}

Client::Message Client::RecieveMsg(SOCKET p_conn) {
    Message message;
    string fullmsg = "";
    char headerbuff[MESSAGE_HEADER] = { 0 };
    char messagebuff[MESSAGE_LENGTH] = { 0 };

    int tnb = 0;
    int nb = 0;
    int length = 0;

    while (tnb < 21) {
        nb = recv(p_conn, &headerbuff[tnb], MESSAGE_HEADER, 0);

        if (nb == 0) {
            cerr << "Server has closed it's connection" << endl;
            closesocket(p_conn);
            exit(EXIT_FAILURE);
        }
        else if (nb == SOCKET_ERROR) {
            cerr << "Recv returned an error with error code: " << WSAGetLastError() << endl;
            closesocket(p_conn);
            exit(EXIT_FAILURE);
        }

        tnb += nb;

        if (tnb == 21) {
            string temp = "";
            temp.append(headerbuff);
            length = stoi(temp.substr(18, 3));
        }
    }

    nb = 0;
    tnb = 0;

    while (tnb < length) {
        nb = recv(p_conn, &messagebuff[tnb], MESSAGE_LENGTH, 0);

        if (nb == 0) {
            cerr << "Server has closed it's connection" << endl;
            closesocket(p_conn);
            exit(EXIT_FAILURE);
        }
        else if (nb == SOCKET_ERROR) {
            cerr << "Recv returned an error with error code: " << WSAGetLastError() << endl;
            closesocket(p_conn);
            exit(EXIT_FAILURE);
        }

        tnb += nb;
    }

    fullmsg.append(headerbuff);
    fullmsg.append(messagebuff, length);

    message = ParseMsg(fullmsg);

    return message;
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
    
    int i = 0;
    while (p_message[i] != '\0') {
        message.username += p_message[i];
        i++;
    }

    message.type = p_message[17] - 48;
    message.length = stoi(p_message.substr(18, 3));
    message.content = p_message.substr(21, message.length);

    return message;
}

void Client::LaunchDisplay() {
    return;
}

void Client::ClientToServer() {
    cout << "You have successfully entered the chat room." << endl;
    Sleep(1000);
    cout << "The message limit is 280 characters." << endl;
    Sleep(1000);
    cout << "You may now begin sending messages." << endl;

    string input;
    Message message;
    message.username = m_username;
    message.type = 1;

    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input.size() > 0 && input.size() <= 280) {
            message.length = input.size();
            message.content = input;
            SendMsg(m_chatConn, message);
            cout << endl;
        }
        else {
            cerr << "Message is empty or exceeded max length. Try again." << endl;
        }
    }
}

void Client::ServerToDisplay() {
    Message message;

    while (true) {
        message = RecieveMsg(m_chatConn);
        string temp = ConvertToMsg(message);
        cout << temp << endl;
        //SendMsg(m_displayConn, message);
    }
}

Client::~Client() {
    cout << "Performing cleanup" << endl;

    cout << "Exiting client application " << endl;
    cout << "Good-bye!" << endl;
}