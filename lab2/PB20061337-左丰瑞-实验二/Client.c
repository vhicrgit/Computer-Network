#include<stdio.h>
#include<string.h>
#include<time.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 5201//服务器端口
SOCKET sockClient;//用于通信的socket，设置为全局变量保证所有函数可调用

void send_msg();//发送信息函数
void recv_msg();//发送信息函数

//光标控制相关变量
HANDLE hConsole;
CONSOLE_SCREEN_BUFFER_INFO csbi;

int main()
{
    WSADATA wsaData;
    WORD wVersionRequested;
    int ret;
    //初始化
    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0){
        printf("初始化失败\n");
        return 0;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){
        printf("加载失败\n");
        WSACleanup();
        return 0;
    }

    HANDLE thread1, thread2;
    SOCKADDR_IN addr;
    sockClient = socket(AF_INET, SOCK_STREAM, 0);//创建套接字
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//使用回环网址
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_family = AF_INET;
    
    printf("正在请求链接服务器中......\n");//连接服务器
    while (1) {
        if (connect(sockClient, (SOCKADDR*)&addr, sizeof(SOCKADDR)) == 0)
            break;
    }
    printf("**************** 聊天室接入成功 ****************\n");
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    

    //创建两个线程，分别用于接受数据和发送数据
    thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_msg, NULL, 0, NULL);
    thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recv_msg, NULL, 0, NULL);
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);
    shutdown(sockClient, 2);
    closesocket(sockClient);//关闭套接字
    return 0;
}

void send_msg()
{
    char date[32];
    char context[128];
   
    while (1) {
        printf("\n客户端: ");
        gets(context);
        char message[256] = "";
        /* 生成时间戳 */
        time_t tt = time(0);
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&tt));

        //获取当前光标位置，上移一行打印聊天信息，以覆盖输入的字符
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        SetConsoleCursorPosition(hConsole, (COORD){0,csbi.dwCursorPosition.Y - 1});
        printf("\r%*s\r", 90, " ");

        printf("客户端: %s\n%s\n", date, context);

        strcat(message, date);
        strcat(message, "\n");
        strcat(message, context);
        send(sockClient, message, strlen(message) + 1, 0);
    }
}


void recv_msg()
{
    char buf[256];
    //接收服务发送的数据
    while (1)
    {
        int n;
        if (recv(sockClient, buf, 256, 0) > 0) {
            printf("\r");
            printf("%s%s\n", "服务器: ", buf);
            printf("\n客户端:");
        }
        else {
            break;
        }
    }
}
