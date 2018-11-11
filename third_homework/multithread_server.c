/*
*Name:multithread_server.c
*
*Function:Use Thread-Specific Data to solve the safe problem of multithread. This server 
*can connect serveral client at the same time and send the reverse string to client.
*
*Author:weekdawn
*
*Time:2018.11.09 17:20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/wait.h>

#define PORT 15000//端口号
#define BACKLOG 5//最大连接个数
#define MAXDATA 1000//最大传输数据

void process_cli(int connectfd, struct sockaddr_in client);
void *start_routine(void *arg);

typedef struct ARG{
	int connfd;
	struct sockaddr_in client;
} ARG;

pthread_key_t key;

int main(void)
{
    int listen_fd, connect_fd;//分别为监听套接字和连接套接字
	
    pthread_t tid;
	ARG arg;//用于线程传递参数
	
    struct sockaddr_in server_addr, client_addr;
    int sin_size;
    //建立socket
    listen_fd=socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd == -1)
    {
      perror("Create socket failed.");
      exit(-1);
    }else
    {
      printf("Create socket...\n");
    }
	
    //bind端口和通配IP
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
      perror("Bind failed.");
      exit(-1);
    }else
    {
  	  printf("Bind...\n");
    }
	
    //开启listen
    if(listen(listen_fd, BACKLOG) == -1)
    {
      perror("Listen failed.");
      exit(-1);
    }else
    {
    printf("Listen...\n");
    }
  
    sin_size = sizeof(struct sockaddr_in);

	
	while(1) 
	{ 
		//在这里阻塞直到接收到消息，参数分别是listen_fd句柄，接收到的地址信息以及大小
		connect_fd=accept(listen_fd,(struct sockaddr*)&client_addr,&sin_size);
		if(connect_fd == -1)
		{
			perror("Accept error.\n");
			exit(-1);
		}else
		{
			printf("Accept...\n");
		}
		
		arg.connfd = connect_fd;
		arg.client = client_addr;
		//创建线程
		if(pthread_create(&tid, NULL, start_routine, (void*)&arg))
		{
		  perror("pthread create error.\n");
		  exit(-1);
		}

	}
	
    close(listen_fd);
}

void *start_routine(void *arg)
{
	ARG *info = (ARG*)arg;
	process_cli(info -> connfd, info -> client);
	pthread_exit(NULL);
}

void process_cli(int connect_fd, struct sockaddr_in client)
{
	int recv_num;
	char recvbuf[MAXDATA], sendbuf[MAXDATA], cli_name[MAXDATA];//接收数据，发送数据，客户端名称
	int index = 0;//保存客户输入字符串的长度，用TSD进行保护
	
	//创建线程专用数据TSD
  	printf("init pthread key.\n");
	if(pthread_key_create(&key, NULL))
	{
		printf("pthread_key_create failed.\n");
	}
	
	printf("You got a connection from %s : %d.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	recv_num = recv(connect_fd, cli_name, MAXDATA, 0);
	cli_name[recv_num] = '\0';
	if(recv_num == -1)
	{
		close(connect_fd);
		printf("client failed.\n");
		return;
	}
	
	printf("Client name is %s.\n", cli_name);
	while(recv_num = recv(connect_fd, recvbuf, MAXDATA,0))
	{
		recvbuf[recv_num] = '\0';		
		
		if(!strcmp(recvbuf,"exit"))
		{
			printf("%s has been exit.\nAll receive data(%s):%s\n\n", cli_name, cli_name, sendbuf);
		    break;
		}
		
		printf("Receive client(%s) message(%d): %s\n", cli_name, recv_num, recvbuf);
		
		//存储TSD数据
		pthread_setspecific(key, &index);
		for(int i=0; i < recv_num; i++)
		{
			sendbuf[*(int *)pthread_getspecific(key)] = recvbuf[i];
			index = *(int *)pthread_getspecific(key) + 1;
		}
		sendbuf[index] = '\0';
		
		send(connect_fd, sendbuf, strlen(sendbuf), 0);
	}
	//删除线程专用数据TSD
	pthread_key_delete(key);
	close(connect_fd);
	
}
