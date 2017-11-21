#include<stdio.h>
#include "winsock2.h"

int main() {
    //Khoi tao winsock
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData)==SOCKET_ERROR)
    {
        printf("Status:\t\t Khoi tao Winsock that bai.");
        system("PAUSE");
        return -1;
    }

    //Tao socket cho FTP Client
    SOCKET ftpControl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Cau hinh dia chi FTP Server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(21);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Ket noi toi FTP Server
    if (connect(ftpControl,(sockaddr*)&addr, sizeof(addr))==SOCKET_ERROR)
    {
        closesocket(ftpControl);
        WSACleanup();
        printf("Status:\t\t Khong ket noi duoc FTP Server.");
        system("PAUSE");
        return -1;
    }



    WSACleanup();
    return 0;
}