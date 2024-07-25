#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "mouse_movement.h"

int main(int argc, char *argv[])
{
	if (argv[1]==0)
	{
	   printf("Usage: fpen [ip]");
		exit(1);
	}

	int port = 6969;
	int bufferSize = 50;
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	char buffer[bufferSize];
	char *ip = argv[1];

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

	strcpy(buffer,"0");
	sendto(sockfd,
			buffer,
			bufferSize,
			0,
			(struct sockaddr*)&addr,
			addr_size);

	float x = 0, y = 0, z = 0, ax, ay, az;
	float sf = 10; /* scaling factor */

	int mousefd = setup_uinput_device();	
	if (mousefd == -1)
	{
		perror("setup_uinput_device() error");
		exit(EXIT_FAILURE);
	}
	
	while (1)
	{
		recvfrom(sockfd,buffer,bufferSize,0,(struct sockaddr*)&addr,&addr_size);
		buffer[bufferSize] = '\0';

		char *token = strtok(buffer, ",");
		ax = atof(token);
		if (fabs(ax) > 0.1)
			x = (x - ax * sf);

		token = strtok(NULL, ",");
		ay = atof(token);
		if (fabs(ay) > 0.1)
			y = (y - ay * sf );

		token = strtok(NULL, ",");
		az = atof(token);
		if (fabs(az) > 0.1)
			z = (z - az * sf);
		
		printf("x = %f, y = %f, z = %f | x = %f, y = %f, z = %f\n", x, y, z, ax, ay, az);

		move_mouse(sockfd, x, y);
		
		//usleep(10000);
	}

	destroy_uinput_device(mousefd);
	return 0;
}
