#include<stdio.h>
#include<string.h>
#include<time.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 5201//�������˿�
SOCKET sockClient;//����ͨ�ŵ�socket������Ϊȫ�ֱ�����֤���к����ɵ���

void send_msg();//������Ϣ����
void recv_msg();//������Ϣ����

//��������ر���
HANDLE hConsole;
CONSOLE_SCREEN_BUFFER_INFO csbi;

int main()
{
    WSADATA wsaData;
    WORD wVersionRequested;
    int ret;
    //��ʼ��
    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0){
        printf("��ʼ��ʧ��\n");
        return 0;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){
        printf("����ʧ��\n");
        WSACleanup();
        return 0;
    }

    HANDLE thread1, thread2;
    SOCKADDR_IN addr;
    sockClient = socket(AF_INET, SOCK_STREAM, 0);//�����׽���
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//ʹ�ûػ���ַ
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_family = AF_INET;
    
    printf("�����������ӷ�������......\n");//���ӷ�����
    while (1) {
        if (connect(sockClient, (SOCKADDR*)&addr, sizeof(SOCKADDR)) == 0)
            break;
    }
    printf("**************** �����ҽ���ɹ� ****************\n");
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    

    //���������̣߳��ֱ����ڽ������ݺͷ�������
    thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_msg, NULL, 0, NULL);
    thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recv_msg, NULL, 0, NULL);
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);
    shutdown(sockClient, 2);
    closesocket(sockClient);//�ر��׽���
    return 0;
}

void send_msg()
{
    char date[32];
    char context[128];
   
    while (1) {
        printf("\n�ͻ���: ");
        gets(context);
        char message[256] = "";
        /* ����ʱ��� */
        time_t tt = time(0);
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&tt));

        //��ȡ��ǰ���λ�ã�����һ�д�ӡ������Ϣ���Ը���������ַ�
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        SetConsoleCursorPosition(hConsole, (COORD){0,csbi.dwCursorPosition.Y - 1});
        printf("\r%*s\r", 90, " ");

        printf("�ͻ���: %s\n%s\n", date, context);

        strcat(message, date);
        strcat(message, "\n");
        strcat(message, context);
        send(sockClient, message, strlen(message) + 1, 0);
    }
}


void recv_msg()
{
    char buf[256];
    //���շ����͵�����
    while (1)
    {
        int n;
        if (recv(sockClient, buf, 256, 0) > 0) {
            printf("\r");
            printf("%s%s\n", "������: ", buf);
            printf("\n�ͻ���:");
        }
        else {
            break;
        }
    }
}
