#include <iostream>
#include <winsock2.h>

// g++ -o server server.cpp -lws2_32
// Command to compile the code, linking the Winsock library.

int main() {
    // Initialize WSA (Winsock) variables
    WSADATA wsaData; // This struct stores information about the Windows Sockets implementation.
    int wsaerr; // Used to store error codes from Winsock function calls.
    WORD wVersionRequested = MAKEWORD(2, 2); // This specifies the version of Winsock (2.2) to use.
    wsaerr = WSAStartup(wVersionRequested, &wsaData); // Initializes Winsock and loads necessary DLLs.

    // Check if WSAStartup was successful
    if (wsaerr != 0) {
        // If there’s an error, print the error code and exit.
        std::cerr << "WSAStartup failed with error: " << wsaerr << std::endl;
        return 0;
    }
    else {
        // If successful, print a status message.
        std::cout << "WSAStartup successful" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl; // Display system status from WSADATA.
    }

    SOCKET serverSocket; // This will store the server's socket.
    serverSocket = INVALID_SOCKET; // Initialize with an invalid state.
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
    socket() creates a socket for communication.
    AF_INET: Address Family for IPv4
    SOCK_STREAM: Specifies the use of TCP (connection-based protocol)
    IPPROTO_TCP: Explicitly specifies TCP protocol
    */

    if (serverSocket == INVALID_SOCKET) {
        // If socket creation fails, print the error and clean up Winsock.
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup(); // Frees any resources used by Winsock.
        return 0;
    }
    else {
        // If socket creation is successful, print a success message.
        std::cout << "Socket created successfully" << std::endl;
    }

    // Bind the socket
    sockaddr_in service; // Structure for specifying the address to bind the socket to.
    service.sin_family = AF_INET; // Address family for IPv4.
    service.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address to bind to, here it's localhost.
    service.sin_port = htons(55555); // Port number to bind to (55555). htons() ensures correct byte order.

    // Bind the socket
    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
        // bind() associates the socket with a specific IP and port. If it fails, print an error.
        std::cerr << "bind() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket); // Close the socket.
        WSACleanup(); // Clean up Winsock.
        return 0;
    }
    else {
        // If bind() is successful, print a success message.
        std::cout << "bind() is OK!" << std::endl;
    }

    // Now the server will start listening for incoming connections.
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        // listen() makes the socket listen for incoming connection requests.
        std::cerr << "listen() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket); // Close the socket.
        WSACleanup(); // Clean up Winsock.
        return 0;
    }
    else {
        std::cout << "listen() is OK!" << std::endl;
    }

    // Accept connections from clients.
    SOCKET acceptSocket;
    acceptSocket = accept(serverSocket, nullptr, nullptr);
    /*
    accept() waits for a connection from a client. Once a client connects, accept() returns a new socket
    specifically for communication with that client.
    */

    if (acceptSocket == INVALID_SOCKET) {
        std::cerr << "accept() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket); // Close the server socket if accept fails.
        WSACleanup(); // Clean up Winsock.
        return 0;
    }
    else {
        std::cout << "accept() is OK!" << std::endl;
    }

    /*
    Now that a client is connected, the server will receive and send data.
    */

    char recieveBuffer[200]; // Buffer for storing received data from the client.

    int recieveByteCount = recv(acceptSocket, recieveBuffer, 200, 0);
    /*
    recv() reads data from the client socket. Here, it reads 200 bytes into the recieveBuffer.
    */
    if (recieveByteCount < 0) {
        std::cerr << "Server recv error: " << WSAGetLastError() << std::endl;
        return 0;
    }
    else {
        std::cout << "Received " << recieveBuffer << " bytes" << std::endl;
    }

    // Loop for continuous data transmission
    while (true) {
        std::string message;
        std::cin >> message; // Get a message from the user.

        if (message == "send") {
            // Prepare to send data to the client.
            char sendBuffer[200];
            std::cout << "Enter: ";
            std::cin.getline(sendBuffer, 200); // Get the data from the user to send to the client.

            int sendByteCount = send(acceptSocket, sendBuffer, 200, 0);
            /*
            send() sends the contents of sendBuffer to the client.
            */
            if (sendByteCount < 0) {
                std::cerr << "Server send error: " << WSAGetLastError() << std::endl;
                return 0;
            }
            else {
                std::cout << "Sent " << sendByteCount << " bytes" << std::endl;
            }

        }
        else if (message == "exit") {
            // If user enters "exit", break out of the loop and terminate.
            break;
        }
    }

    return 0;
}