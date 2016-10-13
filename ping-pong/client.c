#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#include <time.h>
struct timespec timer1, timer2;

void error_handling(char* message);

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int sock;
char message[5120000];
int msg_len;
int cnt = 0;
int rounds = 0;

int read_full();
int write_full();

int main(int argc, char **argv) {
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;
    int i;
    memset(&serv_addr, 0, sizeof(serv_addr));

    sscanf(argv[3], "%d", &msg_len);
    sscanf(argv[4], "%d", &rounds);
    if (argc != 5)
    {
        printf("Usage: /%s <IP> <Port> <msg_len> <rounds>\n",argv[0]);
        exit(1);
    }
    for (i = 0; i < msg_len; i++) {
        message[i] = 'A';
    }
    message[0] = 'B';
    message[i] = 0;


    //创建用于internet的流协议(TCP)socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    //设置一个socket地址结构client_addr,代表客户机internet地址, 端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    //把socket和socket地址结构联系起来
    if( connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    /*str_len = read(sock, message, msg_len);
    if (str_len == -1) {
        error_handling("read() error");
    }
    message[str_len] = 0;


    printf("Message(%d) from server : %s \n", str_len, message);
    printf("SS\n");
    */

    while(rounds--) {
        clock_gettime(CLOCK_REALTIME, &timer1);
        write_full();
        read_full();
        clock_gettime(CLOCK_REALTIME, &timer2);
        double duration = (timer2.tv_sec - timer1.tv_sec) * 1e6 + (timer2.tv_nsec - timer1.tv_nsec) / 1e3;
        duration /= 2.0;
        double bandwidth = msg_len * 8.0 / (duration);
        printf("Round %4d: %6.3lfMbps %8.8lfus\n", cnt++, bandwidth, duration);
    }

    close(sock);
    return 0;
}

int read_full()
{
    int rlen = 0;
    int tmp = 0;
    do {
        tmp = read(sock, message + rlen, msg_len - rlen);
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
        tmp = write(sock, message + wlen, msg_len - wlen);
        wlen += tmp;
#ifdef DEBUG
        printf("wlen:%d\n", wlen);
#endif
    } while (wlen < msg_len && tmp > 0);

    return 0;
}

