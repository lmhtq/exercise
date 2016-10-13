#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char* message);

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int serv_sock;
int clnt_sock;
char message[5120000];
int msg_len;

int read_full();
int write_full();

int main(int argc, char **argv)
{
    int i;
    //设置一个server地址serv_addr,client地址clnt_addr
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&serv_addr, 0, sizeof(serv_addr));
    sscanf(argv[2], "%d", &msg_len);
    for (i = 0; i < msg_len; i++) {
        message[i] = 'A';
    }
    message[0] = 'B';
    message[i] = 0;

    if (argc != 3) {
        printf("Usage : %s <port> <msg_len>\n", argv[0]);
        exit(1);
    }

    //创建用于internet的流协议(TCP)socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    //把socket和socket地址结构联系起来
    if( bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    //server_socket用于监听
    if ( listen(serv_sock, 5) == -1) {
        error_handling("lisen() error");
    }

    // 受理连接请求
    clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
    if ( clnt_sock == -1) {
        error_handling("accept() error");
    }

    //write(clnt_sock, message, msg_len);
    while (1) {
        read_full();
        write_full();
    }
    close(clnt_sock);
    close(serv_sock);

    return 0;
}

int read_full()
{
    int rlen = 0;
    int tmp = 0;
    do {
        tmp = read(clnt_sock, message + rlen, msg_len - rlen);
        rlen += tmp;
#ifdef DEBUG
        printf("rlen:%d\n", rlen);
#endif
    } while (rlen < msg_len && tmp > 0);

    return 0;
}

int write_full()
{
    int wlen = 0;
    int tmp = 0;
    do {
        tmp = write(clnt_sock, message + wlen, msg_len - wlen);
        wlen += tmp;
#ifdef DEBUG
        printf("wlen:%d\n", wlen);
#endif
    } while (wlen < msg_len && tmp > 0);

    return 0;
}

