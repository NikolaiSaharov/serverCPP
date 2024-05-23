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
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для соединения с сервером
    const char* sendBuffer1 = "Hello from client"; // Буфер для первого сообщения, которое будет отправлено серверу
    const char* sendBuffer2 = "Second message from client"; // Буфер для второго сообщения, которое будет отправлено серверу

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

    // Получение информации об адресе сервера
    result = getaddrinfo("localhost", "6565", &hints, &addrResult);// подключаемся прописывая порт и вызывая переменные в которых указывали с адресом и настройками сервера
    if (result != 0) {
        cout << "getaddrinfo failed" << endl;
        WSACleanup();
        return 1;
    }

    // Создание сокета для соединения с сервером
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Error at socket()" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult

    // Отправка первого сообщения серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Sent: " << sendBuffer1 << endl; // Вывод отправленного сообщения на консоль

    // Отправка второго сообщения серверу
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Sent: " << sendBuffer2 << endl; // Вывод отправленного сообщения на консоль

    closesocket(ConnectSocket); // Закрытие сокета для соединения с сервером
    WSACleanup(); // Очистка ресурсов Windows Sockets

    return 0;
}