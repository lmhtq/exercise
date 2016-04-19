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

int sock_make(void);
void read_from(int fd);
void send_to(int fd);

int main(void)
{
	int sockfd,connfd;
	struct sockaddr_in clientaddr;
	int n;
	char buf[BUF_SIZE];
	char recvbuf[BUF_SIZE];
	sockfd=sock_make();
	connfd=accept(sockfd,(struct sockaddr*)NULL,NULL);

	while(1){int num=1,sum=0,rs=1;
		while(rs){
			num=recv(connfd,recvbuf,sizeof(recvbuf),0);
			printf("num:%d\n",num);
			sum=sum+num;
			printf("sum:%d\n",sum);
			printf("recvbuf:%d\n",sizeof(recvbuf));
			if(num<0){ 
				if(errno==EAGAIN)break; else continue;
			}else if(num==0){ }
			if(num!=sizeof(recvbuf)) rs=1;else rs=0;
			if(sum==BUF_SIZE){
				printf("recv~~~\n");
				send(connfd,"1",sizeof("1"),0);
				break;
			}  
		} 
	}
	close(connfd);
	close(sockfd);
	return 0;
}
int sock_make(void)
{
	struct sockaddr_in clientaddr;
	int sockfd;
	memset(&clientaddr,SOCK_STREAM,sizeof(clientaddr));
	clientaddr.sin_family=AF_INET;
	clientaddr.sin_port=htons(LISTEN_PORT);
	clientaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		err_exit(">>socket");
	}

	if(bind(sockfd,(struct sockaddr*)&clientaddr,sizeof(clientaddr))<0)
	{
		err_exit(">>bind");
	}

	listen(sockfd,LISTEN_NO);

	return sockfd;
}

