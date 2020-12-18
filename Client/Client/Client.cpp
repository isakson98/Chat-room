#include "Client.h"

/*
NAME
        Client::Client - Declares all uninitialized client member variables
SYNOPSIS
        Client::Client();
DESCRIPTION
        Default constructor for the client class. Initializes all member variables.
AUTHOR
        Milos Miladinov
*/
Client::Client() {
    m_username = "";
    m_password = "";
    m_host = "";
    m_chatConn = NULL;
    m_displayConn = NULL;
}

/*
NAME
        Client::StartUp - Client Initialization
SYNOPSIS
        void Client::StartUp();
DESCRIPTION
        This function prepares the client to start sending and receiving messages. The proper
        windows library is verified, a connection to the server is established, the user's
        credentials are confirmed by the server, and the chat display is launched.
AUTHOR
        Milos Miladinov
*/
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

    //LaunchDisplay();
    //m_displayConn = EstablishTCPConn("127.0.0.1", m_displayService);
}

/*
NAME
        Client::InIt - Initializes the use of Ws2_32.dll
SYNOPSIS
        void Client::InIt();
DESCRIPTION
        This function checks to see if the correct version of the Ws2_32.dll library is available.

        Error checking confirmes if the requested version of the windows library is found on the
        user's machine.
AUTHOR
        Milos Miladinov
*/
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

/*
NAME
        Client::AskForIp - Asks the user if the user is local or remote
SYNOPSIS
        string Client::AskForIP();
DESCRIPTION
        The user is prompted about the location of the server. Localhost is used if the user
        selects local. If the user selects remote, they are prompted for the IPv4 address of
        the server.

        Error checking is provided to guarantee that a remote address is in valid IPv4 format.
RETURNS
        The server's IPv4 address
AUTHOR
        Milos Miladinov
*/
string Client::AskForIP() {
    string userDecision;
    string host;

    cout << "Where is the server located?: " << endl;
    cout << "(1) Locally" << endl;
    cout << "(2) Remotely" << endl;

    // Indefinetly loops until the user provides a valid response for the server location
    while (true) {
        getline(cin, userDecision);
        cout << endl;

        // User chose local
        if (userDecision == "1" || userDecision == "Locally") {
            host = "127.0.0.1";
            return host;
        }

        // User chose remote
        else if (userDecision == "2" || userDecision == "Remotely") {
            cout << "Enter the server IP: ";

            // Indefintely loops until the user provides a valid IPv4 address for the remote server
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

/*
NAME
        Client::EstablishTCPConn - Creates a TCP connection to a server
SYNOPSIS
        SOCKET Client::EstablishTCPConn(string p_host, string p_service);

        p_host -> The server's IPv4 address
        p_service -> The port to connect to
DESCRIPTION
        Using the host address and port, a TCP connection is established with the server. 

        Error checking ensures that the address and port are valid. The connection to the
        server is also verified.
RETURNS
        The successfully connected network socket
AUTHOR
        Milos Miladinov
*/
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

/*
NAME
        Client::AskForCredentials - Prompts the user for their login credentials
SYNOPSIS
        void Client::AskForCredentials();
DESCRIPTION
        The user is asked for their login credentials. The username and password given
        are then stored as member variables inside the client class for later use. 

        Error checking ensures that the username and password provided are of the appropriate
        length.
AUTHOR
        Milos Miladinov
*/
void Client::AskForCredentials() {
    string username = "";
    string password = "";

    cout << "Please enter your login credentials." << endl;
    cout << "Username (Max length - 16 characters): ";

    // Indefinetely loops until the user provides a username with a length in the range
    // of 1-16 characters.
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

    // Indefinetely loops until the user provides a password with a length in the range
    // of 1-280 characters. The password can have a max length of a standard chat message.
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

/*
NAME
        Client::Authenticate - Verifies the login credentials with the server
SYNOPSIS
        bool Client::Authenticate(string p_username, string p_password);

        p_username -> The user provided username
        p_password -> The user provided password
DESCRIPTION
        The username and password provided by the user are sent as a message to the server and
        verified. If the verification succeeds, the server will send back a confirmation in
        message header. The same is true if the verification fails.

        Error checking is used to check the message type header for the verification status
        of the login credentials.
RETURNS
        A boolean value for the status of the login verification
AUTHOR
        Milos Miladinov
*/
bool Client::Authenticate(string p_username, string p_password) {
    Message login;
    login.username = m_username;
    login.type = 0;
    login.length = m_password.size();
    login.content = m_password;

    SendMsg(m_chatConn, &login);

    login = ReceiveMsg(m_chatConn);

    if (login.type == MESSAGE_CONFIRM) {
        return true;
    }
    else {
        cerr << "Login credentials are invalid. Please try again." << endl << endl;
        return false;
    }
}

/*
NAME
        Client::SendMsg - Sends a message to a server
SYNOPSIS
        void Client::SendMsg(SOCKET p_conn, Message* p_message);

        p_conn -> The socket connection to a server
        p_message -> The message to send to the server
DESCRIPTION
        The function first populates the buffer in the message struct by converting the message
        into a valid character array. The amount of bytes in this buffer that are sent to the server
        is equivalent to the length of the message + the 21 bytes in the message header.

        Error checking is used to make sure the message is sent successfully.
AUTHOR
        Milos Miladinov
*/
void Client::SendMsg(SOCKET p_conn, Message* p_message) {
    // The buffer in the given message struct is built
    ConvertToMsg(p_message);

    if (send(p_conn, p_message->message, p_message->length + MESSAGE_HEADER, 0) == SOCKET_ERROR) {
        cerr << "Send returned an error with error code: " << WSAGetLastError() << endl;
        closesocket(p_conn);
        exit(EXIT_FAILURE);
    }
}

/*
NAME
        Client::ReceiveMsg - Receives a message from a server
SYNOPSIS
        Client::Message Client::ReceiveMsg();
DESCRIPTION
        This function recieves messages from the chatroom server in two parts. The first loop
        extracts the header data, including the message length. The second loop uses the message
        length to extract the message itself. The resulting buffers are then parsed and converted
        into a message.

        Error checking is used to verify that bytes sent from the server are all properly received.
RETURNS
        The message received from the server
AUTHOR
        Milos Miladinov
*/
Client::Message Client::ReceiveMsg(SOCKET p_conn) {
    Message message;
    char headerbuff[MESSAGE_HEADER];
    char messagebuff[MESSAGE_LENGTH];

    int tnb = 0;
    int nb = 0;
    int length = 0;

    // Bytes are continously received until the entire message header is obtained
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

        // Once the entire message header is received, the message length is parsed out
        // of the header.
        if (tnb == MESSAGE_HEADER) {
            char temp[3];
            memcpy(temp, &headerbuff[18], 3);
            length = atoi(temp);
        }
    }

    nb = 0;
    tnb = 0;

    // Bytes are continously received until the entire message is obtained
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

/*
NAME
        Client::ConvertToMsg - Builds a char buffer out of the message which is then able
        to be sent to the server
SYNOPSIS
        void Client::ConvertToMsg(Message* p_message)

        p_message -> The message that has it's buffer modified
DESCRIPTION
        This function takes in a message and copies all of it's data into a char buffer
        built into the message struct. The buffer is formatted based on the agreed upon
        message standard to allow the server to properly read it.
AUTHOR
        Milos Miladinov
*/
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

/*
NAME
        Client::ParseMsg - Converts the buffer data into a usable message struct
SYNOPSIS
        Client::Message Client::ParseMsg(char* p_header, char* p_message, int p_length);

        p_header -> The buffer containing the header data
        p_message -> The buffer containing the message data
        p_length -> The length of the message
DESCRIPTION
        This function parses the data in the received buffers and stores them into a message struct.
        the data is parsed into the message sender, message type, message length, and message content.
RETURNS
        The parsed message data
AUTHOR
        Milos Miladinov
*/
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

/*
NAME
        Client::ClientToServer - The user enters a message to send to the chatroom server
SYNOPSIS
        void Client::ClientToServer();
DESCRIPTION
        The user is prompted to enter a message that they want to send to the chat. A message struct
        is then constructed containing the user's message and information. The data is then sent
        to the server.

        Error checking ensures that the message given is of the appropriate length.
AUTHOR
        Milos Miladinov
*/
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

    // Indefinetely loops in order to continously take in user input
    while (true) {
        cout << "> ";
        getline(cin, input);

        // Message length must be in the range of 1-280
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

/*
NAME
        Client::ServerToDisplay - Received messages are sent to be displayed
SYNOPSIS
        void Client::ServerToDisplay();
DESCRIPTION
        Messages are continously read in from the chat server and sent to the display.
AUTHOR
        Milos Miladinov
*/
void Client::ServerToDisplay() {
    Message message;

    while (true) {
        message = ReceiveMsg(m_chatConn);
        //SendMsg(m_displayConn, &message);
    }
}

/*
NAME
        Client::~Client - Destructs client
SYNOPSIS
        Client::~Client();
AUTHOR
        Milos Miladinov
*/
Client::~Client() {
    cout << "Performing cleanup" << endl;

    cout << "Exiting client application " << endl;
    cout << "Good-bye!" << endl;
}
