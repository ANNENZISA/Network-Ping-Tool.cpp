#include <iostream>
#include <string>
#include <cstdlib>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool ping(const string& ipAddress) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock.\n";
        return false;
    }

    SOCKET icmpSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (icmpSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket.\n";
        WSACleanup();
        return false;
    }

    sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress.c_str(), &(destAddr.sin_addr));

    // Prepare ICMP Echo Request packet
    char icmpPacket[32];
    memset(icmpPacket, 0, sizeof(icmpPacket));
    icmpPacket[0] = 0x08; // ICMP type for Echo Request

    // Send ICMP Echo Request
    if (sendto(icmpSocket, icmpPacket, sizeof(icmpPacket), 0, (sockaddr*)&destAddr, sizeof(destAddr)) == SOCKET_ERROR) {
        cerr << "Failed to send ICMP Echo Request.\n";
        closesocket(icmpSocket);
        WSACleanup();
        return false;
    }

    // Receive ICMP Echo Reply
    char replyBuffer[128];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    if (recvfrom(icmpSocket, replyBuffer, sizeof(replyBuffer), 0, (sockaddr*)&senderAddr, &senderAddrSize) == SOCKET_ERROR) {
        cerr << "No response received.\n";
        closesocket(icmpSocket);
        WSACleanup();
        return false;
    }

    closesocket(icmpSocket);
    WSACleanup();

    cout << "Ping successful. " << ipAddress << " is reachable.\n";
    return true;
}

int main() {
    string ipAddress;

    cout << "Enter IP address or domain to ping: ";
    cin >> ipAddress;

    if (ping(ipAddress)) {
        return 0;
    } else {
        return 1;
    }
}
