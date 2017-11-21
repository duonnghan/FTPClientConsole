#include "HanDuong.h"

int Create(SOCKET ftpControl) {
    char cmd[1024];
    int len;
    char pathServer[1024], buf[1024];
    
    puts("Nhap duong dan cho thu muc moi: ");
    fflush(stdin);
    gets_s(pathServer);

    sprintf(cmd,"mkdir %s",pathServer);
    send(ftpControl, cmd, strlen(cmd),0);

    int res = recv(ftpControl,buf, sizeof(buf), 0);
    if (res == SOCKET_ERROR)
    {
        printf("Status:\t\t Tao moi thu muc that bai.\n");
        return -1;
    }
    len = strlen(buf);
    buf[len] = 0;
    printf("Status:\t\t %s\n", buf);
    return 0;
}
int Rename(SOCKET ftpControl) {
    char cmd[1024];
    char newName[512], oldName[512];
    int len;
    char pathServer[1024], buf[1024];

    puts("Nhap duong dan toi thu muc: ");
    fflush(stdin);
    gets_s(pathServer);

    puts("Nhap ten moi: ");
    fflush(stdin);
    gets_s(newName);

    sprintf(cmd, "cs %s| mv %s %s", pathServer, oldName, newName);
    send(ftpControl, cmd, strlen(cmd),0);

    int res = recv(ftpControl, buf, sizeof(buf), 0);
    len = strlen(buf);
    buf[len] = 0;
    printf("Status:\t\t %s", buf);
    return 0;
}
int Delete(SOCKET ftpControl) {

    return 0;
}