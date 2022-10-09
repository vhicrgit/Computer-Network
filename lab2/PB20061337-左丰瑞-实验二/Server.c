#include<stdio.h>
#include<string.h>
#include<time.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 5201//�������˿�

void send_msg();//������Ϣ����
void recv_msg();//������Ϣ����

HANDLE hConsole;
CONSOLE_SCREEN_BUFFER_INFO csbi;

SOCKET clientSocket;
int main() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret;
    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret == -1) {
        printf("error");
        return 0;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("����ʧ��\n");
        WSACleanup();
        return 0;
    }

    HANDLE thread1, thread2;
    SOCKET serverSocket;
    SOCKADDR_IN addr;
    SOCKADDR_IN clientAddr;
    memset(&addr, 0, sizeof(addr));
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//ʹ��TCPЭ�鴫��
    addr.sin_family = AF_INET;//ʹ��IPv4��ַ
    addr.sin_addr.S_un.S_addr = htonl(ADDR_ANY);//���������ַ
    addr.sin_port = htons(SERVER_PORT);//���ö˿�
    bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
    listen(serverSocket, 3);//��ʼ����
    printf("�ȴ��ͻ��˽�����......\n");
    int len = sizeof(SOCKADDR);
    while (1) {
        clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &len);
        printf("**************** �ͻ��˽���ɹ� ****************\n");
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_msg, NULL, 0, NULL);
        thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recv_msg, NULL, 0, NULL);
    }
    closesocket(clientSocket);
    return 0;
}

void send_msg() {
    char date[32];
    char context[128];
    while (1) {
        printf("\n��������");
        gets(context);
        char message[256] = "";
        /* ����ʱ��� */
        time_t tt = time(0);
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&tt));

        GetConsoleScreenBufferInfo(hConsole, &csbi);
        SetConsoleCursorPosition(hConsole, (COORD) { 0, csbi.dwCursorPosition.Y - 1 });
        printf("\r%*s\r", 90, " ");

        printf("������: %s\n%s\n", date, context);

        strcat(message, date);
        strcat(message, "\n");
        strcat(message, context);
        send(clientSocket, message, strlen(message) + 1, 0);
    }
}

void recv_msg() {
    char buf[256];
    while (1) {
        int len;
        len = recv(clientSocket, buf, 256, 0);
        if (len < 0) {
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\r");
            printf("�ͻ������˳�����������׼������\n");
            break;
        }
        else if (len == 0)
            printf("ERROR!!");
        else {
            printf("\r");
            printf("%s%s\n", "�ͻ���: ", buf);
            printf("\n������:");
        }
    }
}