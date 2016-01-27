#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <event.h>

#define BUF_SIZE 1024
#define ERROR    (-1)
#define TRUE     (1)
#define FALSE    (0)

char ip[20] = "127.0.0.1";
int port    = 8198;

int connet_sever(char *ip, int port)
{
	int ret;
	/* create socket */
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket failed");
		exit(ERROR);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
	
	/* connet to server */
	ret = connect(fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	if (ret < 0) {
		close(fd);
		perror("connect falied");
		exit(ERROR);
	}

	return fd;
}

void handle_read_event(int sock, short event, void *arg)
{
	char buf[BUF_SIZE];
	int recv_len;

	recv_len = read(sock, buf, BUF_SIZE);
	if (recv_len == 0) {
		/* socket closed */
		return;
	}
	printf("Recv:%d\n", recv_len);
}

void* init_read_event(void *arg)
{
	long long_sock = (long)arg;
	int sock = (int)long_sock;

	/* init libeventm set callback functions */
	struct event_base* base = event_base_new();
	struct event* read_ev = (struct event*)calloc(1, sizeof(struct event));
	
	event_set(read_ev, sock, EV_READ|EV_PERSIST, handle_read_event, NULL);
	event_base_set(base, read_ev);
	event_add(read_ev, NULL);
	event_base_dispatch(base);

	event_del(read_ev);
	free(read_ev);
	event_base_free(base);
}

void init_read_event_thread(int sock)
{
	pthread_t thread;
	pthread_create(&thread, NULL, init_read_event, (void*)sock);
}

int main()
{
	int socket_fd;
	char buf[BUF_SIZE];
	int write_len;

	socket_fd = connet_sever(ip, port);
	printf("socket_fd:%d\n", socket_fd);
	init_read_event_thread(socket_fd);

	while (1) {
		printf("Input:");
		scanf("%s", buf);
		if (strcmp("q", buf) == 0) {
			close(socket_fd);
			break;
		}
		write_len = write(socket_fd, buf, strlen(buf));
		sleep(2);
	}
	printf("Done!\n");
	return 0;
}
