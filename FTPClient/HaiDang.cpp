#include "HaiDang.h"


char dir[256];		//duong dan tep
int upload(SOCKET ftpControl)
{
    SOCKET dataControl = CreateSocketData(ftpControl);
    int res;
    char pathServer[1024], pathClient[1024], buf[102400];
    char command[128], cmd[512];
    printf("\nNhap duong dan cua file tai len: ");
    scanf("%s", pathClient);
    printf("\nNhap duong dan tren sever: ");
    scanf("%s", pathServer);
    sprintf(command, "STOR %s\n", pathServer);
    send(ftpControl, command, strlen(command), 0);//dung tra ve ma 150
    res = recv(ftpControl, cmd, 512, 0);
    cmd[res] = '\0';
    puts(cmd);
    if (strncmp(cmd, "150", 3) == SOCKET_ERROR) {
        puts("kiem tra lai duong dan tren server");
        closesocket(dataControl);
        return 0;
    }
    FILE * f;
    fopen_s(&f, pathClient, "rb");
    if (f == NULL) {
        puts("kiem tra lai duong dan file client");
        closesocket(dataControl);
        return 0;
    }
    else {
        while (true) {
            res = fread(buf, 1, 102400, f);
            if (res == 0)
                break;
            send(dataControl, buf, res, 0);
            if (res < 102400)
                break;
        }
        fclose(f);
        closesocket(dataControl);
        res = recv(ftpControl, cmd, 512, 0);
        cmd[res] = '\0';
        puts(cmd);
    }

    return 0;
}

int show(SOCKET ftpControl)
{
    SOCKET dataControl = CreateSocketData(ftpControl);
    char buf[1024];
    int res;
    char msg[512];
    char databuf[5120];
    send(ftpControl, "PWD\r\n", strlen("PWD\r\n"), 0);		//Yeu cau xem thu muc hien tai
    int len = recv(ftpControl, buf, sizeof(buf), 0);
    buf[len] = '\0';
    sscanf(buf, "257 \"%[^\"]\" ", dir);//doc cho đến khi gặp đóng ngoặc kép
    puts(dir);
    printf("Danh sach file cua thu muc %s: \n", dir);
    sprintf(msg, "LIST %s\r\n", dir);
    send(ftpControl, msg, strlen(msg), 0);			//Yeu cau liet ke danh sach tep cua thu muc hien tai
    len = recv(ftpControl, buf, sizeof(buf), 0);
    buf[len] = '\0';
    puts(buf);
    len = recv(ftpControl, buf, sizeof(buf), 0);
    buf[len] = '\0';
    puts(buf);
    puts("bat dau nhan du lieu");
    while (true)
    {    
        res = recv(dataControl, databuf, 512, 0);
        if (res == SOCKET_ERROR || res == 0)
            break;
        databuf[res] = '\0';
        printf("%s", databuf);
    }
    closesocket(dataControl);
    return 0;
}

int Quit(SOCKET ftpControl)
{
    char*msg = "QUIT\r\n";		//Yeu cau ngat ket noi toi Server.
    send(ftpControl, msg, strlen(msg), 0);
    printf("Ban vua ngat ket noi toi FTP Server\n");
    return 0;
}
//download 1 file 1 luồng.
int download(SOCKET ftpControl)
{
    SOCKET dataControl = CreateSocketData(ftpControl);
    int res;
    char buf[512];
    char cmd[512];
    char pathServer[128];
    char pathClient[128];
    puts("nhap duong dan file muon download:");
    //gets_s(pathServer);
    //fflush(stdin);
    scanf("%s", pathServer);
    puts("nhap duong dan se luu tren client:");
    /*gets_s(pathClient);
    fflush(stdin);*/
    scanf("%s", pathClient);
    sprintf(cmd, "RETR %s\n", pathServer);//dung se tra ve ma 150 Opening data channel for file download
    send(ftpControl, cmd, strlen(cmd), 0);
    res = recv(ftpControl, buf, 512, 0);
    buf[res] = '\0';
    puts(buf);
    if (strncmp(buf, "150", 3) != 0) {
        printf("\nkiem tra lai duong dan %s", pathServer);
        closesocket(dataControl);
        return false;
    }
    FILE *f = fopen(pathClient, "wb");
    if (f == NULL) {
        printf("\nkiem tra lai duong dan tren client %s", pathClient);
        closesocket(dataControl);
        return false;
    }
    char dataBuffer[102400];
    while (true) {
        res = recv(dataControl, dataBuffer, 102400, 0);
        if (res == SOCKET_ERROR || res == 0)
            break;
        fwrite(dataBuffer, 1, res, f);
    }
    res = recv(ftpControl, buf, 512, 0);//download thanh cong tra ve ma  226 Successfully transferred "/video.mp4"

    buf[res] = '\0';
    puts(buf);
    fclose(f);
    closesocket(dataControl);
    return 0;
}


SOCKET CreateSocketData(SOCKET ftpControl)
{
    char buf[512];
    int dataIp[4];
    int dataPort[2];
    char strIp[32];
    send(ftpControl, "TYPE I\r\n", strlen("TYPE I\r\n"), 0);
    int len = recv(ftpControl, buf, sizeof(buf), 0); //yeu cau gui du lieu kieu binary
    char command[128] = "PASV\r\n";
    send(ftpControl, command, strlen(command), 0);
   
    int res = recv(ftpControl, buf, 512, 0);
    buf[res] = '\0';
    puts(buf);
    //kiem tra co phai ma loi 227
    if (strncmp(buf, "227", 3) != 0) {
        return SOCKET_ERROR;
    }
    int rev = sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r", &dataIp[0], &dataIp[1], &dataIp[2], &dataIp[3], &dataPort[0], &dataPort[1]);
    sprintf(strIp, "%d.%d.%d.%d\0", dataIp[0], dataIp[1], dataIp[2], dataIp[3]);
    puts(strIp);
    int dataP = dataPort[0] * 256 + dataPort[1];
    printf("%d", dataP);
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
        getchar();
        return 0;
    }
    SOCKET dataControl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN addrServer;
    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_port = htons(dataP);
    addrServer.sin_family = AF_INET;
    if (connect(dataControl, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }
    puts("Ket noi thanh cong datachanel");
    return dataControl;
}


//chuyen thu muc lam viec
int cwd(SOCKET ftpControl)
{
    char buf[512];
    char msg[512];
    printf("Di chuyen toi thu muc moi.\n");
    printf("Nhap duong dan: \n");
    scanf("%s", dir);
    sprintf(msg, "CWD %s\r\n", dir);		//Yeu cau di chuyen den thu muc khac
    send(ftpControl, msg, strlen(msg), 0);
    int len = recv(ftpControl, buf, 128, 0);
    buf[len] = '\0';
    puts(buf);
    if (buf[0] == '5')						//Khong co thu muc do, hoac la mot tep
        printf("Duong dan khong hop le\n");
    else
        printf("Da di chuyen toi thu muc %s\n", dir);
    WSACleanup();
    return 0;
}
