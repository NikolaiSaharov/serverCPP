#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>

using namespace std;

int main()
{
    WSADATA wsaData; // Структура для хранения информации о версии Windows Sockets
    ADDRINFOA hints; // Структура для хранения настроек сокета
    ADDRINFO* addrResult; // Указатель на структуру, которая будет содержать информацию об адресе сервера
    char recvBuffer[512]; // Буфер для принятых данных
    SOCKET ListenSocket = INVALID_SOCKET; // Сокет для прослушивания входящих соединений
    SOCKET ClientSocket = INVALID_SOCKET; // Сокет для обслуживания клиента

    // Инициализация Windows Sockets
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed" << endl;
        return 1;
    }

    // Очистка структуры hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // Семейство адресов (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Тип сокета (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Протокол (TCP)
    hints.ai_flags = AI_PASSIVE; // Сокет будет использоваться для приема входящих соединений

    // Получение информации об адресе сервера
    result = getaddrinfo(NULL, "6565", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed" << endl;
        WSACleanup();
        return 1;
    }

    // Создание сокета для прослушивания входящих соединений
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Error at socket()" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Привязка сокета к адресу сервера
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "bind failed with error" << endl;
        freeaddrinfo(addrResult);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult

    // Начало прослушивания входящих соединений
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen failed with error" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Принятие входящего соединения
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accept failed" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket); // Закрытие сокета для прослушивания входящих соединений

    // Цикл для приема данных от клиента
    do {
        result = recv(ClientSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Bytes received: " << result << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closing..." << endl;
        }
        else {
            cout << "recv failed" << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Завершение отправки данных клиенту
    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "shutdown failed with error: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket); // Закрытие сокета для клиента
    WSACleanup(); // Очистка ресурсов Windows Sockets

    return 0;
}