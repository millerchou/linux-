#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define CLTMAX 256
#define CHATSIZE 256
void * chat(void * arg);
void deliver(char * msg, int len);
void errordealer(char * msg);

int count = 0;
int cltsock[CLTMAX];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int ssock, csock, cltsize;
	struct sockaddr_in saddr, caddr;
	pthread_t t1;
	if (argc != 2) {
		printf("请输入标准格式： %s <端口号>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL);
	ssock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(atoi(argv[1]));

	if (bind(ssock, (struct sockaddr*) &saddr, sizeof(saddr)) == -1)
		errordealer("bind 函数发生错误");
	if (listen(ssock, 5) == -1)
		errordealer("listen 函数发生错误");

	while (1)
	{
		cltsize = sizeof(caddr);
		csock = accept(ssock, (struct sockaddr*)&caddr, &cltsize);

		pthread_mutex_lock(&mutx);
		cltsock[count++] = csock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t1, NULL, chat, (void*)&csock);
		pthread_detach(t1);
		printf("连接到客户端 IP: %s \n", inet_ntoa(caddr.sin_addr));
	}
	close(ssock);
	return 0;
}

void * chat(void * arg)
{
	int csock = *((int*)arg);
	int len = 0, i;
	char msg[CHATSIZE];

	while ((len = read(csock, msg, sizeof(msg))) != 0)
		deliver(msg, len);

	pthread_mutex_lock(&mutx);
	for (i = 0; i<count; i++)   
	{
		if (csock == cltsock[i])
		{
			while (i++<count - 1)
				cltsock[i] = cltsock[i + 1];
			break;
		}
	}
	count--;
	pthread_mutex_unlock(&mutx);
	close(csock);
	return NULL;
}
void deliver(char * msg, int len)   
{
	int i;
	pthread_mutex_lock(&mutx);
	for (i = 0; i<count; i++)
		write(cltsock[i], msg, len);
	pthread_mutex_unlock(&mutx);
}

void errordealer(char * msg)
{
	fputs(msg, stderr);
	exit(1);
}