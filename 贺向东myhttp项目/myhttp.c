#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<signal.h>
#include<wait.h>
#include<sys/types.h>
#include<fcntl.h>
#include<pthread.h>
int get_fd(char buff[1024],int c)
{
    // 使用 strtok 函数以空格为分隔符，获取第一个标记，即请>求方法
        char* s = strtok(buff, " ");
        // 如果没有获取到标记，关闭套接字 c，继续下一次循环
        if (s == NULL) {
            close(c);
            return -1;
        }
        // 打印请求方法
printf("请求方法：%s\n", s);

        // 再次使用 strtok 函数获取下一个标记，即请求的资源
        s = strtok(NULL, " ");
        // 如果没有获取到标记，关闭套接字 c，继续下一次循环
        if (s == NULL) {
            close(c);
            return -1;
        }
        // 打印请求的资源
        printf("请求的资源：%s\n", s);
        // 如果请求的资源是根路径（"/"），则设置为默认的资源路径（"/index.html"）
        if (strcmp(s, "/") == 0) {
            s = "/index.html";
        }
        // 定义一个字符数组用于存储资源文件的路径
        char path[128] = {"/home/ubuntu/myhttp"};
        // 将请求的资源路径拼接到基础路径后面
        strcat(path, s);
        // 以只读方式打开资源文件，返回文件描述符
        int fd = open(path, O_RDONLY);
        return fd;
}

int create_socket()
{
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        return -1;
    }
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(80);
    saddr.sin_addr.s_addr=inet_addr("49.233.173.242");
   int res= bind(sockfd,(struct sockaddr *)&saddr,sizeof(saddr));
    if(res==-1)
   {
       return -1;
   }
   res=listen(sockfd,5);
   if(res==-1)
   {
       return -1;
   }
return sockfd;
}


void *fun(void *arg)
{
    int c=*(int *)arg;
    while(1)
    {

        char buff[1024]={0};
        int n=recv(c,buff,1023,0);
        if(n<=0)
        {
            break;
        }
        printf("n=%d,read:\n",n);
        printf("%s\n",buff);
        int fd=get_fd(buff,c);
        if(fd==-1)
        {
char path_404[128]={"/home/ubuntu/myhttp/my404.html"};
            int fd_404=open(path_404,O_RDONLY);
            if(fd_404=-1)
            {

                close(c);
                continue;
            }
            int size_404=lseek(fd_404,0,SEEK_END);
            lseek(fd_404,0,SEEK_SET);
            char sendbuff[512]={0};
             // 构造 HTTP 响应头的第一行，表示成功响应（这里其实是模拟成功响应，实际上是发送自定义的 404 页面）
            strcpy(sendbuff, "HTTP/1.1 200 OK\r\n");
            strcat(sendbuff, "Server:myhttp_quzijie\r\n");
            // 将文件大小拼接到响应头中，表示响应体的长度
            sprintf(sendbuff + strlen(sendbuff), "Content-Length:%d\r\n", size_404);
            strcat(sendbuff, "\r\n");

            // 打印要发送的响应头内容，用于调试
            printf("send:\n%s\n", sendbuff);
            // 向客户端发送响应头
send(c, sendbuff, strlen(sendbuff), 0);
            char data[512] = {0};
            int num = 0;
            // 循环读取 404.html 文件内容并发送给客户端
            while ((num = read(fd_404, data, 512)) > 0) {
            send(c, data, num, 0);
            }
            // 关闭 404.html 文件描述符和客户端连接描述符
            close(fd_404);
            close(c);




        }
        int size=lseek(fd,0,SEEK_END);
        lseek(fd,0,SEEK_SET);


        // send(c,"hello quzijie!\n",15,0);
        char sendbuff[512]={0};
        strcpy(sendbuff,"HTTP/1.1 200 OK\r\n");
        strcat(sendbuff,"Server:myhttp_quzijie\r\n");
// strcat(sendbuff,"Content-Length:14\r\n");
        sprintf(sendbuff+strlen(sendbuff),"Content-Length:%d\r\n",size);

        strcat(sendbuff,"\r\n");
        // strcat(sendbuff,"");

        printf("send:\n%s\n",sendbuff);
        send(c,sendbuff,strlen(sendbuff),0);
        char data[512]={0};
        int num=0;
        while((num=read(fd,data,512))>0)
        {
            send(c,data,num,0);
        }
        close(fd);
        close(c);

    }

}

int main()
{
    int sockfd=create_socket();
   // assert(sockfd!=-1);
    struct sockaddr_in caddr;
    int len=-1;
    int c=-1;
    while(1)
    {
        len=sizeof(caddr);
        c=accept(sockfd,(struct sockaddr *)&caddr,&len);
        printf("accept wait...\n");
        if(c<0)
        {
            continue;
        }
        printf("accept c=%d\n",c);
        pthread_t id;
        pthread_create(&id,NULL,fun,(void *)&c);
    }
}

