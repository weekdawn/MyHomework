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

int main(void)
{
  int listen_fd, connect_fd;//分别为监听套接字和连接套接字
	
  pthread_t tid;
  struct ARG *arg;
	
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
	
  //开启listen，第二个参数为最大连接数
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
      perror("Accept error.");
      exit(-1);
    }else
	{
	  printf("Accept...\n\n");
	}

	ARG arg;
	arg.connfd = connect_fd;
	arg.client = client_addr;
	//memcpy((void*)&arg -> client, &client_addr, sizeof(client_addr));
	  
	if(pthread_create(&tid, NULL, start_routine, (void*)&arg))
	 {
	 	perror("pthread create error.\n");
		exit(-1);
	 }
	  
  } 

  close(listen_fd);
}


void process_cli(int connect_fd, struct sockaddr_in client)
{
	int num;
	char recvbuf[MAXDATA], sendbuf[MAXDATA], cli_name[MAXDATA];
	printf("You got a connection from %s : %d.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	num = recv(connect_fd, cli_name, MAXDATA, 0);
	if(num == 0)
	{
		close(connect_fd);
		printf("client disconneted.\n");
		return;
	}
	
	cli_name[num] = '\0';
	printf("Client name is %s.\n", cli_name);
	while(num = recv(connect_fd, recvbuf, MAXDATA,0))
	{
		recvbuf[num] = '\0';
		printf("Receive client(%s) message: %s\n", cli_name, recvbuf);
		if(!strcmp(recvbuf,"exit"))
		{
			printf("%s has been exit.\n", cli_name);
		    break;
		}
		//反转接收到的字符串
		for(int i=0; i < num; i++)
			sendbuf[i] = recvbuf[num -i -1];
		sendbuf[num] = '\0';
		send(connect_fd, sendbuf, strlen(sendbuf), 0);
	}
	close(connect_fd);
	
}


void *start_routine(void *arg1)
{
	ARG *info = (ARG*)arg1;
	process_cli(info -> connfd, info -> client);
	pthread_exit(NULL);
}


























