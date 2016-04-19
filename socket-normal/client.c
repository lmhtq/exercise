#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


//#define BUF_SIZE 1048576
#define BUF_SIZE 14400
#define LISTEN_PORT 8889
#define LISTEN_NO 10
#define err_exit(msg) (perror(msg),(exit(EXIT_FAILURE)))

int main(int argc,char * argv[])
{
	int sockfd;
	int n;
	struct sockaddr_in serverAddr;
	char sendbuf[BUF_SIZE];
	char recvbuf[BUF_SIZE];

	if((sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
	{   
		err_exit(">>socket");
	}   
	memset(&serverAddr,0,sizeof(serverAddr));

	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(LISTEN_PORT);
	inet_pton(PF_INET,argv[1],&serverAddr.sin_addr.s_addr);


	if((connect(sockfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))<0)
	{
		err_exit(">>connect");
	}else{
		while(1){
			memset(sendbuf,'a',BUF_SIZE);//initial
			sendbuf[0]='b';
			//int num=rand()%91+10;
			//memset(sendbuf,'b',num);
			//send(sockfd,sendbuf,sizeof(sendbuf),0);
			send(sockfd,sendbuf,sizeof(sendbuf),0);

			recv(sockfd,recvbuf,sizeof(recvbuf),0);
			printf("%s\n",recvbuf);
			sleep(1);
		}
	}
	close(sockfd);
	return 0;
}

