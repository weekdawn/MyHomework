/*
*Name:tcp_client.c
*
*Function:As a simple tcp client, read ip from command line, and connect to the server.
*
*Author:weekdawn
*
*Time:2018.11.05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 15000
#define MAXDATA 100

int main(int argc, char *argv[])
{
	int sockfd,numbytes;
	char buf[MAXDATA];
	struct hostent * he;
	struct sockaddr_in server_addr;
	
	//命令行读入服务器地址
	if(argc != 2)
	{
		printf("Usage:%s < IP address>\n", argv[0]);
		exit(-1);
	}
	
	if((he = gethostbyname(argv[1])) == NULL)
	{
		perror("Gethostbyname error.");
		exit(-1);
	}
	
	//建立socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Create socket fialed.");
		exit(-1);
	}
	
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr = *((struct in_addr *)he -> h_addr);//dest_addr.sin_addr.s_addr=inet_addr(DEST_IP);
	
	//与服务器建立连接
	if(connect(sockfd,(struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("connect failed.");
		exit(-1);
	}
	
	//接收数据
	if((numbytes = recv(sockfd, buf, MAXDATA, 0)) == -1)
	{
		perror("recv error.");
		exit(-1);
	}
	
	buf[numbytes] = '\0';
	printf("Server Message: %s\n", buf);
	
	close(sockfd);
		
}
