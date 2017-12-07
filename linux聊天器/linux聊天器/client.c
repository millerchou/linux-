#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#define CHATSIZE 100
#define NAMESIZE 20
void * outmsg(void*tmp);
void * inmsg(void*tmp);
void errordealer(char*msg);

char name[NAMESIZE] = "[DEFAULT]";
char msg[CHATSIZE];

int main(int ac, char*av[])
{
	int fd;
	struct sockaddr_in addr;
	pthread_t t1, t2;
	void*status;

	if (ac != 4) {
		printf("请输入正确格式：%s <ip> <端口> <姓名>\n", av[0]);
		exit(1);
	}
	sprintf(name, "[%s]", av[3]);
	fd = socket(PF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(av[1]);
	addr.sin_port = htons(atoi(av[2]));

	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) 
		errordealer("connect错误");
	
	pthread_create(&t1, NULL, outmsg, (void*)&fd);
	pthread_create(&t2, NULL, inmsg, (void*)&fd);
	pthread_join(t1, &status);
	pthread_join(t2, &status);
	close(fd);
	return 0;
}
void * outmsg(void * tmp)
{
	int sock = *((int*)tmp);
	char allmsg[NAMESIZE + CHATSIZE];
	while (1)
	{
		fgets(msg, CHATSIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			puts("登出");
			close(sock);
			exit(0);
		}
		sprintf(allmsg, "%s %s", name, msg);
		write(sock, allmsg, strlen(allmsg));
	}
	return NULL;
}
void * inmsg(void * tmp)
{
	int sock = *((int*)tmp);
	char allmsg[NAMESIZE + CHATSIZE];
	int len;
	while (1)
	{
		len = read(sock, allmsg, NAMESIZE + CHATSIZE - 1);
		if (len == -1)
			return (void*)-1;
		allmsg[len] = 0;
		fputs(allmsg, stdout);
	}
	return NULL;
}
void errordealer(char * msg)
{
	fputs(msg, stderr);
	exit(1);
}