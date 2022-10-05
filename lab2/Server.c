#include<stdio.h>
#include<string.h>
#include<WinSock2.h>
#pragma comment (lib,"ws2_32.lib")
#pragma comment(lib, "ws2_32")
SOCKET sockConn;//全局变量，用来通信的套接字
//把发送消息和接收信息的功能封装成两个函数作为线程使用。
void recvFun();//接收信息线程
void sendFun();//发送信息线程
void createConnect();//创建通信套接字
int flag=0;
HANDLE h1, h2;//线程句柄
int main()
{

    SOCKET serverSocket;//监视的套接字
    //SOCKET sockConn;//用来通信的套接字
    SOCKADDR_IN newAddr;//保存客户端的socket地址信息
    SOCKADDR_IN addr;//地址结构体，包括ip port(端口)
    WSADATA data;//存储被WSAStartup函数调用后返回的Windows Sockets数据
    WORD version;//socket版本
    int info;

    //在使用socket之前要进行版本的设定和初始化
    version = MAKEWORD(2, 2);//设定版本
    info = WSAStartup(version, &data);
    /*应用程序或DLL只能在一次成功的WSAStartup()调用之后
    才能调用进一步的Windows Sockets API函数。
    根据版本初始化 windows socket,返回0表示成功
    */
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
    //创建套接字，使用TCP协议
    //有套接字的接口才能进行通信
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//AF_INET使用IPV4地址，SOCK_STREAM使用流传输，IPPROTO_TCP使用TCP协议
    addr.sin_addr.S_un.S_addr = htonl(ADDR_ANY);//表示任何的ip过来连接都接受
    addr.sin_family = AF_INET;//使用ipv4的地址
    addr.sin_port = htons(0606);//设定应用占用的端口
    //用bind()函数确定socket各种属性
    bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));//将套接字serverSocket与端口0606，接收的ip绑定
    listen(serverSocket, 3);//开始监听，是否有客服端请求连接,最大连接数为3
    printf("开始监听，等待客户端连接..........\n");
    int len = sizeof(SOCKADDR);
    //accept是一个阻塞函数，如果没有客户端请求，连接会一直等待在这里
    //该函数会返回一个新的套接字，这个新的套接字是用来与客户端通信的套接字，之前那个套接字是监听的套接字
    while(1){
         sockConn=accept(serverSocket,(SOCKADDR*)&newAddr,&len);//接受客户端的请求
         printf("连接成功......\n");
         //创建线程后立即运行
         //第一个参数表示线程内核对象的安全属性；第二个参数表示线程栈空间大小；第三个参数表示新线程所执行的线程函数地址（函数的名字），多个线程可以使用同一个函数地址
         //第四个参数是传递给线程函数的参数；第五个参数指定什么时候调用线程，为0表示线程创建之后就可以进行调用；第六个参数返回线程的ID号，传入NULL表示不需要返回该线程ID号
         h1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendFun, NULL, 0, NULL);//用于发送的线程
         h2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvFun, NULL, 0, NULL);//用于接收的线程
     }
    closesocket(sockConn);//关闭套接字
    return 0;
}
void sendFun()
{
    char buf[128];
    while (1)
    {
        printf("服务器端的信息:");
        scanf("%s", buf);
        //sockConn为用来通信的套接字，buf为发送数据的缓冲区，strlen(buf)+1为发送数据的长度，0位flags标志
        send(sockConn, buf, strlen(buf) + 1, 0);//发送信息
    }
}

void recvFun()
{
    char buf[128];
    while (1)
    {
        int Ret = recv(sockConn, buf, 128, 0);//接收信息
        if(Ret<0){
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            printf("客户端已退出，服务器端准备监听\n");
            flag=flag%2+1;
            break;
        }else if(Ret==0){
            printf("ERROR_RECV");
        }else{
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            printf("%s%s\n","客户端的信息: ", buf);
            printf("服务器端的信息:");
        }
    }
}
