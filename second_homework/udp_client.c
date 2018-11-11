/*
*Name:udp_client.c
*
*Function:As a simple udp client, read ip from command line, conmmunicate with the server.
*print the server's response.
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

#define PORT 15001
#define MAXDATA 100

int main(int argc, char* argv[])
{
	int sockfd, sendtofd, recvfromfd;
	char buf[MAXDATA];
	struct hostent* he;
	struct sockaddr_in server_addr, reply_addr;
	
	if(argc != 2)
	{
		printf("Usage: %s <IP address>\n", argv[0]);
		exit(-1);
	}
	
	//命令行接收服务器的IP地址
	if((he = gethostbyname(argv[1])) == NULL)
	{
		perror("Gethostbyname error.");
		exit(-1);
	}else
	{
		printf("Gethostbyname...\n");
	}
	
	//创建socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		perror("Create socket failed.");
		exit(-1);
	}else
	{
		printf("Create socket...\n");
	}
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr = *((struct in_addr*)he -> h_addr);
	
	
	while(1)
	{
		int len;
		printf("Please type your msg ：");
		//scanf("%s", &buf);
		gets(buf);
		sendtofd = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
		
		if(!strcmp(buf, "exit"))
			break;
		
		if(sendtofd == -1)
		{
			perror("Sendto error.");
			exit(-1);
		}
		recvfromfd = recvfrom(sockfd, buf, MAXDATA, 0, (struct sockaddr*)&reply_addr, &len);
		if(recvfromfd == -1)
		{
			perror("Recvfrom error.");
			exit(-1);
		}
		
		if(len != sizeof(struct sockaddr) || memcmp ((const void *)&server_addr, (const void*)&reply_addr, len) != 0)
		{
			printf("Receive message from other server.\n");
			continue;
		}

		buf[MAXDATA] = '\0';
		printf("Received message: %s\n", buf);

	}
	
	close(sockfd);
}



















