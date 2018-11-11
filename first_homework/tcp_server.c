/*
*Name:tcp_server.c
*
*Function:As a simple tcp server, accept the client's connection, and print it's ip and port.
*
*Author:weekdawn
*
*Time:2018.11.05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 15000//端口号
#define BACKLOG 5//最大连接个数

int main(void)
{
  int listen_fd, connect_fd;//分别为监听套接字和连接套接字
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

    //read and request
    printf("You get a connection from %s : %d\n",
		   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));//地址转换函数inet_ntoa
    send(connect_fd, "Welconme to weekdawn's world.\n", 29, 0);
    close(connect_fd);
  } 

  close(listen_fd);
}
