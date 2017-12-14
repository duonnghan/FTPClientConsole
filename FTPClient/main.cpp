
#include "HaiDang.h"
#include "HanDuong.h"



int main() {

    //Display the entry title
    displayTitle();
    printf("KET NOI DEN FTP SERVER:\n");
    SOCKET ftpControl = doConnect();
    
    //Hien thi dang nhap
    doLogin(ftpControl);

    //Variable used for reading the user input
    char option;

    //Variable used for controlling the while loop
    bool isRunning = true;

    while (isRunning)
    {
        system("cls");
        displayTitle();
        printf("MENU:\n");
        printf("---------\n");
        fflush(stdin);
        //Outputs the options to console
        puts(
            "\n[1] Danh sach cac tap tin thu muc hien tai."
            "\n[2] Tai xuong tap tin."
            "\n[3] Tai len tap tin."
            "\n[4] Xoa tap tin (thu muc)."
            "\n[5] Doi ten"
            "\n[6] Tao thu muc moi."
            "\n[7] Chuyen thu muc lam viec."
            "\n[8] Di chuyen tap tin."
            "\n.........."
            "\n[x]Exit");

        printf(">>>>>>>>>Lua chon: ");
        //Reads the user's option
        option = getchar();
        //Selects the course of action specified by the option
        switch (option)
        {
            case '1':
                show(ftpControl);
                system("PAUSE");
                break;
            case '2':
                download(ftpControl);
                system("PAUSE");
                break;
            case '3':              
                upload(ftpControl);
                system("PAUSE");
                break;
            case '4':
                deleteDirFile(ftpControl);
                break;
            case '5':
                renameDirFile(ftpControl);
                break;
            case '6':
                createDir(ftpControl);
                break;
            case '7':
                cwd(ftpControl);
                system("PAUSE");
                break;
            case '8':
                moveFile(ftpControl);
                system("PAUSE");
                break;
            case 'x':
            case 'X':
                //Exit the system
                Quit(ftpControl);
                isRunning = false;
                return 0;
            default:
                //User enters wrong input
                printf("Lua chon khong hop le!! Vui long chon lai.\n");
                break;
            }
    }

    closesocket(ftpControl);
    system("PAUSE");
    WSACleanup();
    return 0;
}