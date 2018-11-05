#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 15001
#define MAXDATA 100



int main(void)
{
	int sockfd;
	struct sockaddr_in server_addr, client_addr;
	int sin_size, num;
	char msg[MAXDATA], rev_msg[MAXDATA];
	
	//建立socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		perror("Create socket failed.");
		exit(-1);
	}else
	{
		printf("Create socket successful!\n");
	}
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//绑定套接字到特定地址
	if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind error.");
		exit(-1);
	}else
	{
		printf("Bind successful!\n");
	}
	
	sin_size = sizeof(struct sockaddr_in);
	while(1)
	{
		num = recvfrom(sockfd, msg, MAXDATA, 0, (struct sockaddr *)&client_addr, &sin_size);
		if(num < 0)
		{
			perror("recvfrom error.");
			exit(-1);
		}
		
		msg[num] = '\0';
		
		
		printf("You have been connected from %s : %d \tmsg : %s\n", 
			    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),msg);
				
		if(!strcmp(msg, "exit"))
			break;
		
		//反转字符串
		for(int i=0; i < num; i++)
			rev_msg[i] = msg[num -i -1];
		rev_msg[num] = '\0';
		
		sendto(sockfd, rev_msg, sizeof(rev_msg), 0, (struct sockaddr*)&client_addr, sin_size);
		
	}
	
	//关闭连接
	close(sockfd);



}
