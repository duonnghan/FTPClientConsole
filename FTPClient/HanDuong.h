
#include <stdio.h>
#include <conio.h>
#include "winsock2.h"
#include "ws2tcpip.h"

//Ham thanh cong tra ve 0, that bai tra ve khac 0
int displayTitle();
SOCKET doConnect();
int doLogin(SOCKET controlSocket);
int createDir(SOCKET ftpControl);
int renameDirFile(SOCKET ftpControl);
int deleteDirFile(SOCKET ftpControl);
int moveFile(SOCKET ftpControl);
