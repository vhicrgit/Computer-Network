#include<stdio.h>
#include<string.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
SOCKET sockClient;//全局变量，用于通信的socket
//两个线程用于接收和发送信息
void sendFun();//发送信息线程
void recvFun();//接收信息线程
int main()
{
    HANDLE h1, h2;//线程句柄，其实就是一串数字用来标识线程对象
    SOCKADDR_IN addr;
    int info;
    WSADATA data;
    WORD version;
    //设定版本，与初始化
    version = MAKEWORD(2, 2);
    info = WSAStartup(version, &data);
    if (info != 0)
    {
        printf("初始化失败\n");
        return 0;
    }
    if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2)
    {
        printf("加载失败\n");
        WSACleanup();
        return 0;
    }
    sockClient = socket(AF_INET, SOCK_STREAM, 0);//创建套接字
                                                   //要连接的服务器的ip,因为现在服务器端就是本机，所以写本机ip
                                                   //127.0.0.1一个特殊的IP地址，表示是本机的IP地址
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    //端口要与服务器相同，不然找不到
    addr.sin_port = htons(0606);
    //用IPV4地址
    addr.sin_family = AF_INET;
    //主动连接服务器
    while(1){
        if(connect(sockClient, (SOCKADDR*)&addr, sizeof(SOCKADDR))==0)
            break;
        else
            ;
    }

    //创建线程后立即执行
    h1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendFun, NULL, 0, NULL);
    h2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvFun, NULL, 0, NULL);
    WaitForSingleObject(h1, INFINITE);//会阻塞，直到线程运行结束
    WaitForSingleObject(h2, INFINITE);
    shutdown(sockClient,2);
    closesocket(sockClient);//关闭套接字
    return 0;
}

void sendFun()
{
    char buf[128];
    while (1)
    {
        printf("客户端的信息: ");
        scanf("%s", buf);
        //发送数据
        send(sockClient, buf, strlen(buf) + 1, 0);
    }
}


void recvFun()
{
    char buf[128];
    //接收服务发送的数据
    while (1)
    {
        int n;
        if(recv(sockClient, buf, 128, 0)>0){
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            printf("%s%s\n","服务器端的信息: ", buf);
            printf("客户端的信息:");
        }
        else{
            break;
        }
    }
}
