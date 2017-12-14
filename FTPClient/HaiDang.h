#pragma once
#include <stdio.h>
#include "winsock2.h"


int upload(SOCKET ftpControl);
int show(SOCKET ftpControl);
int Quit(SOCKET ftpControll);
int download(SOCKET ftpControl);
SOCKET CreateSocketData(SOCKET ftpControl);
int cwd(SOCKET ftpControl);


