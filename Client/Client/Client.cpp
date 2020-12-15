#include "Client.h"

Client::Client() {
    m_username = "";
    m_password = "";
    m_host = "";
    m_chatConn = NULL;
    m_displayConn = NULL;
}

void Client::InIt() {
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
        cerr << "Attempt to connect to server failed: " << p_service << endl;
        exit(EXIT_FAILURE);
    }

    return(s);
}

void Client::StartUp() {
    cout << "Hello! Welcome to the ChatRoom Client." << endl;
    Sleep(2000);
    cout << "Before chatting begins, we need a few things from you." << endl << endl;
    Sleep(2000);

    InIt();

    m_host = AskForIP();

    m_chatConn = EstablishTCPConn(m_host, m_chatService);

    do {
        AskForCredentials();
    } while (Authenticate(m_username, m_password) == false);

    LaunchDisplay();
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
    login.username = m_username;
    login.type = 0;
    login.length = m_password.size();
    login.content = m_password;

    SendMsg(m_chatConn, &login);

    login = ReceiveMsg(m_chatConn);

    if (login.type == 2) {
        return true;
    }
    else {
        cerr << "Login credentials are invalid. Please try again." << endl << endl;
        return false;
    }
}

void Client::SendMsg(SOCKET p_conn, Message* p_message) {
    ConvertToMsg(p_message);

    if (send(p_conn, p_message->message, p_message->length + MESSAGE_HEADER, 0) == SOCKET_ERROR) {
        cerr << "Send returned an error with error code: " << WSAGetLastError() << endl;
        closesocket(p_conn);
        exit(EXIT_FAILURE);
    }
}

Client::Message Client::ReceiveMsg(SOCKET p_conn) {
    Message message;
    char headerbuff[MESSAGE_HEADER];
    char messagebuff[MESSAGE_LENGTH];

    int tnb = 0;
    int nb = 0;
    int length = 0;

    while (tnb < MESSAGE_HEADER) {
        nb = recv(p_conn, &headerbuff[tnb], MESSAGE_HEADER - tnb, 0);

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

        if (tnb == MESSAGE_HEADER) {
            char temp[3];
            memcpy(temp, &headerbuff[18], 3);
            length = atoi(temp);
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

    message = ParseMsg(headerbuff, messagebuff, length);

    return message;
}

void Client::ConvertToMsg(Message* p_message) {
    int count = 0;

    strncpy(p_message->message, p_message->username.c_str(), p_message->username.size());
    count += p_message->username.size();

    p_message->message[count] = '\0';
    count++;

    while (count < 17) {
        p_message->message[count] = '0';
        count++;
    }

    p_message->message[count] = p_message->type + 48;
    count++;

    string length = to_string(p_message->length);
    for (int i = 3 - length.size(); i > 0; i--) {
        p_message->message[count] = '0';
        count++;
    }

    strncpy(&p_message->message[count], length.c_str(), length.size());
    count += length.size();

    strncpy(&p_message->message[count], p_message->content.c_str(), p_message->content.size());
}

Client::Message Client::ParseMsg(char* p_header, char* p_message, int p_length) {
    Message message;
    
    int i = 0;
    while (p_header[i] != '\0') {
        message.username += p_header[i];
        i++;
    }

    message.type = p_header[17] - 48;

    message.length = p_length;
    
    message.content.append(p_message, p_length);

    memcpy(message.message, p_header, MESSAGE_HEADER);
    memcpy(&message.message[MESSAGE_HEADER], p_message, p_length);

    return message;
}

void Client::LaunchDisplay() {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));

    si.cb = sizeof(si);

    ZeroMemory(&pi, sizeof(pi));

    // Note that we are creating a new console window.

    if (!CreateProcess("C:\\Users\\Sal\\CLionProjects\\Chat-room\\cmake-build-debug\\DisplayChat.exe",
                       NULL, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi) )

    {

        cerr << "CreateProcessA - failed" << endl;

    }
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

            SendMsg(m_chatConn, &message);

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
        message = ReceiveMsg(m_chatConn);
        cout << "username: " << message.username << endl;
        cout << "message type: " << message.type << endl;
        cout << "message length: " << message.length << endl;
        cout << "message: " << message.content << endl;
        SendMsg(m_displayConn, &message);
    }
}

Client::~Client() {
    cout << "Performing cleanup" << endl;

    cout << "Exiting client application " << endl;
    cout << "Good-bye!" << endl;
}
