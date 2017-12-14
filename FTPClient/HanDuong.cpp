#include "HanDuong.h"

//Display title when starting program
int displayTitle() {
    char    title[1024];
    FILE    *f;
    int     res;

    fopen_s(&f, "entry.txt", "r");
    res = fread(title, 1, sizeof(title), f);
    if (res < 1024)
    {
        title[res] = 0;
        printf(" %s\n", title);
        printf("\n\n\n");
        return 0;
    }

    return -1;
}


//Thuc hien viec ket noi den FTP Server
SOCKET doConnect() {
    char        host[10];
    int         port;
    char        strPort[5];
    SOCKET      ftpControl;
    addrinfo    *result;        //Chua thong tin phan giai ten mien
    sockaddr_in addrResolved;   //Dia chi phan giai duoc

    //Nhap dia chi/ten mien Host
    printf("- Nhap dia chi IP/ten mien may chu FTP: ");
    fflush(stdin);
    gets_s(host);

    //Nhap dia chi Port
    while (true)
    {
        printf("- Nhap so cong (Port): ");
        fflush(stdin);
        gets_s(strPort);
        
        //so sanh voi cac cong cua FTP: 21 - command, 22- data, 990 - FTPS
        if (strncmp(strPort, "21", 2) != 0 && strncmp(strPort, "22", 2) != 0 && strncmp(strPort, "990", 3) != 0){
            printf("Cong khong hop le. Vui long nhap lai.\n");
        }
        else {  //Neu cong hop le, khoi tao ket noi toi FTP

            //Chuyen doi cong dang chuoi sang dang so
            port = atoi(strPort);

            //Khoi tao winsock
            WSADATA wsData;
            if (WSAStartup(MAKEWORD(2, 2), &wsData) == SOCKET_ERROR)
            {
                printf("[ERROR]: Khoi tao Winsock that bai.");
                system("PAUSE");
                return SOCKET_ERROR;
            }

            //Tao socket cho FTP Client
            ftpControl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            //Cau hinh dia chi FTP Server
            SOCKADDR_IN addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            /*addr.sin_addr.s_addr = inet_addr(host);*/

            //Phan giai ten mien
            int rc = getaddrinfo(host, "ftp", NULL, &result);
            memcpy(&addrResolved, result->ai_addr, result->ai_addrlen);
            addr.sin_addr.s_addr = inet_addr(inet_ntoa(addrResolved.sin_addr));

            //Ket noi toi FTP Server
            if (connect(ftpControl, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
            {
                printf("[ERROR]: Khong ket noi duoc FTP Server.");
                system("PAUSE");
                return SOCKET_ERROR;
            }

            printf("[STAT]: Ket noi thanh cong toi FTP Server(%s):!!\n\n\n", inet_ntoa(addrResolved.sin_addr));

            //Nhan thong tin tu Server
            char receive[1024];
            int res = recv(ftpControl, receive, sizeof(receive), 0);    //Tra ve ma 220-FileZilla Server 0.9.59 beta
            if (res == SOCKET_ERROR) {
                closesocket(ftpControl);
                WSACleanup();
                return SOCKET_ERROR;
            }

            receive[res] = 0;
            puts(receive); 
            break;
        }
    }

    return ftpControl;
}

//Thuc hien dang nhap he thong
int doLogin(SOCKET ftpControl) {
    char    username[256], password[256];
    char    buf[1024];
    int     res;

    printf("DANG NHAP:\n");

    do
    {
        printf("Nhap username: ");
        gets_s(username);

        sprintf(buf, "USER %s\n", username);
        res = send(ftpControl, buf, strlen(buf), 0);

        //Nhan phan hoi tu FTP server
        res = recv(ftpControl, buf, sizeof(buf), 0);

        //Xac thuc thong tin dang nhap (username, mat khau)
        if (strncmp(buf, "33", 2) != 0)
        {
            printf("[ERROR]: Sai ten dang nhap hoac mat khau... Vui long thu lai\n");
        }
        else {
            printf("Nhap password(%s): ", username);
            char    c = _getch();
            int     i = 0;

            while (c != 13)
            {
                password[i] = c;
                printf("*");
                c = _getch();
                i++;
            }
            password[i] = 0;
            printf("\n");

            sprintf(buf, "PASS %s\n", password);
            res = send(ftpControl, buf, strlen(buf), 0);

            //Nhan phan hoi tu FTP server
            res = recv(ftpControl, buf, sizeof(buf), 0);
            if (strncmp(buf, "230", 3) != 0)
                printf("[ERROR]: Dang nhap that bai. Vui long thu lai...\n");
            else {
                system("cls");
                displayTitle();
                printf("Dang nhap thanh cong!!");
                Sleep(500);
                break;
            }
        }
    } while (true);

    return 0;
}


//Tao mot thu muc moi
int createDir(SOCKET ftpControl) {

    char    pathServer[1024];
    char    buf[1024];
    
    printf("TAO THU MUC MOI\n");
    printf("- Nhap duong dan cho thu muc moi: ");
    scanf("%s", pathServer);
    //system("cd");
    sprintf(buf,"MKD %s\r\n",pathServer);

    //Yeu cau tao thu muc moi
    //Tra ve ma 257 neu tao thanh cong
    send(ftpControl, buf, strlen(buf),0);

    int res = recv(ftpControl,buf, sizeof(buf), 0);
    if (res == SOCKET_ERROR)
    {
        printf("[ERROR]:\t\t Tao moi thu muc that bai.\n");
        return -1;
    }

    buf[res] = 0;

    //Neu duong dan sai, khong the tao tep hoac thu muc
    if (buf[0] == '5')
    {
        printf("[ERROR]: Duong dan khong hop le...\n");
        system("PAUSE");
        return -1;
    }
    else {
        printf("[STAT]: Da tao thu muc moi thanh cong!!\n");
        printf("\t -> Duong dan thu muc: %s.\n", pathServer);
        system("PAUSE");
    }
    return 0;
}


int renameDirFile(SOCKET ftpControl) {
    char    cmd[1024];
    char    newName[512];
    char    pathServer[1024], buf[1024];
    char    *filename;

    puts("DOI TEN TAP TIN:");
    printf("- Nhap duong dan toi tap tin can doi ten: ");
    scanf("%s", pathServer);
    strcpy(buf, pathServer);

    filename = strtok(pathServer, "/");
    while (filename != NULL)
    {
        filename = strtok(NULL, "/\n");
    }

    //Gui yeu cau doi ten
    sprintf(cmd, "RNFR %s\n", buf);
    send(ftpControl, cmd, strlen(cmd), 0);
    int res = recv(ftpControl, buf, sizeof(buf), 0);
    buf[res] = 0;

    //puts(buf);
    //Tra ve ma 350 neu file ton tai
    if (strncmp(buf, "350", 3) != 0)
    {
        printf("[ERROR]: Duong dan khong hop le hoac file khong ton tai.\n");
        system("PAUSE");
        return -1;
    }else {
        printf("- Nhap ten moi: ");
        scanf("%s", newName);

        //Noi file vao duong dan thu muc
        if (newName != NULL)
        {
            //strcpy(buf, pathServer);
            if (pathServer[strlen(pathServer) - 1] == '/')
                strcat(pathServer, newName);
            else if (pathServer[strlen(pathServer) - 1] != '/') {
                strcat(pathServer, "/");
                strcat(pathServer, newName);
            }
        }

        //Gui yeu cau doi ten
        sprintf(cmd, "RNTO %s\n", pathServer);
        send(ftpControl, cmd, strlen(cmd), 0);
        res = recv(ftpControl, buf, sizeof(buf), 0);    //Doi ten thanh cong tra ve ma 250
        buf[res] = 0;

        //Neu duong dan sai, khong co quyen xoa tep hoac thu muc khong rong
        if (buf[0] == '5') {
            printf("[ERROR]: Khong the doi ten file.\n");
            system("PAUSE");
            return -1;
        }
        else {
            printf("[STAT]: Doi ten thanh cong.\n");
            system("PAUSE");
        }

    }

    
    return 0;
}


int deleteDirFile(SOCKET ftpControl) {
    char    cmd[64];
    char    buf[1024];
    char    pathServer[1024];
    char    choose[10];

    printf("XOA TAP TIN HOAC THU MUC.\n");
    printf("- Chon doi tuong ban muon xoa:\n");
    printf("\t1. Tap tin.\n");
    printf("\t2. Thu muc.\n");
    printf(">>>>>>>>>Lua chon: ");
    scanf("%s", choose);

    //Xoa tap tin
    if (strncmp(choose,"1",1) == 0)
    {
        printf("- Nhap duong dan tap tin can xoa: ");
        scanf("%s", pathServer);

        printf("Ban co thuc su muon xoa khong? (C/K): ");
        scanf("%s", choose);
        if (strncmp(choose, "C", 1) == 0 || strncmp(choose, "c", 1) == 0 ||
            strncmp(choose, "Co", 2) == 0 || strncmp(choose, "co", 2) == 0 ||
            strncmp(choose, "CO", 2) == 0 || strncmp(choose, "cO", 2) == 0) {
            //Yeu cau xoa
            sprintf(cmd, "DELE %s\r\n", pathServer);
            send(ftpControl, cmd, strlen(cmd), 0);
            int len = recv(ftpControl, buf, sizeof(buf), 0);
            buf[len] = 0;

            //Neu duong dan sai, khong co quyen xoa tep hoac thu muc khong rong
            if (buf[0] == '5') {
                printf("[ERROR]: Duong dan khong hop le hoac khong co quyen xoa file.\n");
                system("PAUSE");
                return -1;
            }
            else {
                printf("[STAT]: Da xoa thanh cong.\n");
                system("PAUSE");
            }
        }
        else {
            printf("[STAT]: Da huy lenh xoa.\n");
            system("PAUSE");
        }

        return 0;
    }

    //Xoa thu muc
    if (strncmp(choose, "2", 1) == 0)
    {
        printf("- Nhap duong dan thu muc can xoa: ");
        scanf("%s", pathServer);

        printf("Ban co thuc su muon xoa khong? (C/K): ");
        scanf("%s", choose);
        if (strncmp(choose, "C", 1) == 0 || strncmp(choose, "c", 1) == 0 ||
            strncmp(choose, "Co", 2) == 0 || strncmp(choose, "co", 2) == 0 ||
            strncmp(choose, "CO", 2) == 0 || strncmp(choose, "cO", 2) == 0) {
            //Yeu cau xoa
            sprintf(cmd, "RMD %s\r\n", pathServer);
            send(ftpControl, cmd, strlen(cmd), 0);
            int len = recv(ftpControl, buf, sizeof(buf), 0);
            buf[len] = 0;

            //Neu duong dan sai, khong co quyen xoa tep hoac thu muc khong rong
            if (buf[0] == '5') {
                printf("[ERROR]: Duong dan khong hop le hoac khong co quyen xoa thu muc.\n");
                system("PAUSE");
                return -1;
            }
            else {
                printf("[STAT]: Da xoa thanh cong.\n");
                system("PAUSE");
            }
        }
        else {
            printf("[STAT]: Da huy lenh xoa.\n");
            system("PAUSE");
        }

        return 0;
    }
    
    printf("[ERROR]: Lua chon khong hop le. Vui long thu lai...\n");
    

    return -1;
}

//Di chuyen tap tin
int moveFile(SOCKET ftpControl) {
    char    sourceDir[1024], desDir[1024];
    char    cmd[1024], buf[1024];
    char    *filename;
    int     res;

    printf("DI CHUYEN TAP TIN:\n");
    printf("- Nhap duong dan den tep can di chuyen: ");
    scanf("%s", sourceDir);

    //Yeu cau di chuyen tep
    sprintf(cmd, "RNFR %s\r\n", sourceDir);
    send(ftpControl, cmd, strlen(cmd), 0);
    res = recv(ftpControl, buf, sizeof(buf), 0);

    buf[res] = 0;
    //Khong co tep, tep khong hop le hoac khong co quyen di chuyen tep
    if (buf[0] == '5')
    {
        printf("[ERROR]: Duong dan khong hop le...\n");
        system("PAUSE");
        return -1;
    }
    else {

        //Phan giai duong dan tep
        //char *strtok(char *str, const char *delim): ham chia chuoi "str" ngan cach boi dau tach "delim"
        filename = strtok(sourceDir, "/\n");
        while (filename != NULL)
        {
            strcpy(buf, filename);
            filename = strtok(NULL, "/\n");
            if (buf != NULL)
                strcpy(sourceDir, buf);
        }

        printf("- Nhap thu muc muon chuyen den: ");
        scanf("%s", desDir);
        filename = strtok(desDir, "\n");

        //Phan giai duong dan tep
        while (filename != NULL)
        {
            strcpy(buf, filename);
            filename = strtok(NULL, "\n");
            if (buf != NULL)
                strcpy(desDir, buf);
        }

        //Noi thu muc dich voi tep can di chuyen
        if (desDir[strlen(desDir) - 1] == '/')
            strcat(desDir, sourceDir);
        else if (desDir[strlen(desDir) - 1] != '/'){
            strcat(desDir, "/");
            strcat(desDir, sourceDir);
        }

        //Yeu cau thu muc dich
        sprintf(cmd, "RNTO %s\r\n", desDir);
        send(ftpControl, cmd, strlen(cmd), 0);
        int len = recv(ftpControl, buf, sizeof(buf), 0);
        buf[len] = 0;

        //Khong co thu muc hoac thu muc khong hop le
        if (buf[0] == '5')						
        {
            printf("Duong dan khong hop le\n");
            return -1;
        }else
            printf("Di chuyen tep thanh cong.\n");
    }

    return 0;
}



