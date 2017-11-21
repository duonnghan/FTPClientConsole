#pragma once
#include <stdio.h>
#include "winsock2.h"

//Ham thanh cong tra ve 0, that bai tra ve khac 0

int Create(SOCKET ftpControl);
int Rename(SOCKET ftpControl);
int Delete(SOCKET ftpControl);
