#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <event.h>

char server_ip[20] = "127.0.0.1";
int server_port = 8198;
#define BUF_SIZE 1024

struct sock_ev_write {
	/* used in on_write() */
	/* destroyed after user's write event */
	struct event* write_ev;
	char buf[BUF_SIZE];
};

struct sock_ev {
	/* used for destroying when read event terminated(socket closed) */
	struct event_base* base;
	struct event *read_ev;
};

inline
void destroy_sock_ev_write(struct sock_ev_write* sock_ev_write_struct)
{
	if (sock_ev_write_struct != NULL) {
		if (sock_ev_write_struct->write_ev != NULL) {
			free(sock_ev_write_struct->write_ev);
		}
		free(sock_ev_write_struct);
	}
}

inline
void destroy_sock_ev(struct sock_ev* sock_ev_struct)
{
	if (sock_ev_struct == NULL) {
		return;
	}
	event_del(sock_ev_struct->read_ev);
	event_base_loopexit(sock_ev_struct->base, NULL);
	if (NULL != sock_ev_struct->read_ev) {
		free(sock_ev_struct->read_ev);
	}
	event_base_free(sock_ev_struct->base);
	free(sock_ev_struct);
}

int get_socket()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket failed");
		exit(-1);
	}
	return fd;
}

void handle_write_event(int sock, short event, void* arg)
{
	char buf[BUF_SIZE];
	int  write_len;
	struct sock_ev_write* sock_ev_write_struct;
	
	printf("handle_write_event called, sock=%d\n", sock);
	if (NULL == arg) {
		perror("arg error");
		exit(-1);
	}
	
	sock_ev_write_struct = 
		(struct sock_ev_write*)arg;
	sprintf(buf, "fd(sock)=%d, received[%s]", sock, sock_ev_write_struct->buf);
	
	write_len = write(sock, buf, strlen(buf));
	destroy_sock_ev_write(sock_ev_write_struct);
	printf("handle_write_event finished, sock(fd)=%d\n", sock);
}

void handle_read_event(int sock, short event, void *arg)
{
	printf("handle_read_event called, sock=%d\n", sock);
	if (arg == NULL) {
		return ;
	}

	struct sock_ev *event_struct;
	char *buf;
	int read_len;
	struct sock_ev_write *sock_ev_write_struct;
	struct event* write_ev;

	event_struct = (struct sock_ev*)arg;
	sock_ev_write_struct = (struct sock_ev_write*)calloc(1, sizeof(struct sock_ev_write));
	buf = sock_ev_write_struct->buf;

	read_len = read(sock, buf, BUF_SIZE);
	if (read_len == 0) {
		printf("read size is 0 for socket:%d\n", sock);
		destroy_sock_ev(event_struct);
		close(sock);
		return;
	}
	
	write_ev = (struct sock_ev_write*)calloc(1, sizeof(struct sock_ev_write));
	sock_ev_write_struct->write_ev = write_ev;

	event_set(write_ev, sock, EV_WRITE, handle_write_event, sock_ev_write_struct);
	event_base_set(event_struct->base, write_ev);
	event_add(write_ev, NULL);
	printf("handle_read_event finished %d.\n", sock);

}

void* process_in_new_thread_when_accepted(void *arg)
{
	long long_fd = (long)arg;
	int fd = (int)long_fd;
	struct event_base* base = event_base_new();
	struct event* read_ev = (struct event*)calloc(1, sizeof(struct event));
	struct sock_ev* event_struct = (struct sock_ev*)calloc(1, sizeof(struct sock_ev));
	
	if (fd < 0) {
		printf("process_in_new_thread_when_accepted() quit!\n");
		return 0;
	}
	
	event_struct->base = base;
	event_struct->read_ev = read_ev;
	event_set(read_ev, fd, EV_READ|EV_PERSIST, handle_read_event, event_struct);
	event_base_set(base, read_ev);
	event_add(read_ev, NULL);
	
	event_base_dispatch(base);
	printf("event_base_dispatch() stopped for sock(fd:%d), in process_in_new_thread_when_accepted()\n", fd);
	return 0;
}

void accept_new_thread(int sock)
{
	pthread_t thread;
	pthread_create(&thread, NULL, process_in_new_thread_when_accepted, (void *)sock);
	pthread_detach(thread);
}

void handle_accept(int sock, short event, void* arg)
{
	struct sockaddr_in remote_addr;
	int sin_size = sizeof(struct sockaddr_in);
	int new_fd = accept(sock, (struct sockaddr*)&remote_addr, (socklen_t*)&sin_size);
	if (new_fd < 0) {
		printf("accept error in handle_accept for sock:%d\n", sock);
		return;
	}
	printf("new_fd accepted by server, new_fd is %d\n", new_fd);
	accept_new_thread(new_fd);
	printf("handle_accept finished for new_fd:%d\n", new_fd);
}

int main()
{
	int fd = get_socket();
	if (fd < 0) {
		printf("Error in main, fd < 0\n");
		exit(-1);
	}

	struct sockaddr_in local_addr;
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = inet_addr(server_ip);
	local_addr.sin_port = htons(server_port);

	int bind_ret = bind(fd, (struct sockaddr*)&local_addr, sizeof(struct sockaddr));
	if (bind_ret < 0) {
		printf("bind error in main()\n");
		exit(-1);
	}

	int backlog = 4096;
	listen(fd, backlog);

	struct event_base* base = event_base_new();
	struct event listen_ev;

	event_set(&listen_ev, fd, EV_READ|EV_PERSIST, handle_accept, NULL);
	event_base_set(base, &listen_ev);
	event_add(&listen_ev, NULL);
	event_base_dispatch(base);

	/* Never goto here in correct cases */
	event_del(&listen_ev);
	event_base_free(base);
	printf("Done!\n");
	return 0;
}
