#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>

// Function to handle receiving messages from the server
void receiveMessages(SOCKET clientSocket) {
    char receiveBuffer[200]; // Buffer for storing received data
    while (true) {
        // Receive data from the server
        int receiveByteCount = recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0);
        if (receiveByteCount > 0) {
            receiveBuffer[receiveByteCount] = '\0'; // Null-terminate the string
            std::cout << "Server: " << receiveBuffer << std::endl; // Print the message from the server
        }
        else if (receiveByteCount == 0) {
            std::cout << "Connection closed by server." << std::endl;
            break;
        }
        else {
            std::cerr << "Client recv error: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

int main() {
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    if (wsaerr != 0) {
        std::cerr << "WSAStartup failed with error: " << wsaerr << std::endl;
        return 0;
    }
    else {
        std::cout << "WSAStartup successful" << std::endl;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    }

    // Connect to the server
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address
    clientService.sin_port = htons(55555); // Server port number

    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService)) == SOCKET_ERROR) {
        std::cerr << "Client: connect() failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    }

    std::cout << "Client: Connected to server. You can now send and receive messages..." << std::endl;

    // Start a new thread for receiving messages
    std::thread receiverThread(receiveMessages, clientSocket);


    // Main thread for sending messages
    while (true) {
        std::string message;
        std::getline(std::cin, message); // Get user input

        if (message == "exit") {
            std::cout << "Closing connection..." << std::endl;
            break; // Exit the loop and close the connection
        }

        // Send the message to the server
        int sendByteCount = send(clientSocket, message.c_str(), message.size(), 0);
        if (sendByteCount == SOCKET_ERROR) {
            std::cerr << "Client send error: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    // Close the socket and clean up
    closesocket(clientSocket);
    WSACleanup();

    // Wait for the receiving thread to finish before exiting
    receiverThread.join();

    return 0;
}
