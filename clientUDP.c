#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <stdio.h>
#include <string.h>

int main(int argc, char*argv[])
{
	if (argv[1]==0)
	{
	   printf("Usage: tcom [ip]");
		exit(1);
	}
	
	int port = 6969;
	int bufferSize = 255;
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	char buffer[bufferSize];
	char *ip = "192.168.31.223";

	struct sockaddr_in addr;

	socklen_t addr_size;
	
	if (sockfd == -1)
	{
		perror("socket() error");
		exit(EXIT_FAILURE);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &addr.sin_addr)<=0)
	{
		perror("inet_pton() error");
		exit(EXIT_FAILURE);
	}
	addr_size = sizeof(addr);

	int x = 0, y =0;

	// TODO - get screen resolution
	int width = 1600;
	int height = 900;

	while (1)
	{
		strcpy(buffer,"0");
		sendto(sockfd,buffer,bufferSize,0,(struct sockaddr*)&addr,addr_size);
		recvfrom(sockfd,buffer,bufferSize,0,(struct sockaddr*)&addr,&addr_size);

		x -= atoi(buffer)/16;
		if (x > width)
		{
			x = width;
		}
		if (x < 0)
		{
			x = 0;
		}

		strcpy(buffer, "1");
		sendto(sockfd,buffer,bufferSize,0,(struct sockaddr*)&addr,addr_size);
		recvfrom(sockfd,buffer,bufferSize,0,(struct sockaddr*)&addr,&addr_size);

		y -= atoi(buffer)/16;
		if (y > height)
		{
			y = height;
		}
		if (y < 0)
		{
			y = 0;
		}

		printf("%d - %d\n",x,y);
	}
	return 0;
}
