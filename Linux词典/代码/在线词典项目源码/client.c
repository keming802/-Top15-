#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define   N  32

#define  R  1   // user - register
#define  L  2   // user - login
#define  Q  3   // user - query
#define  H  4   // user - history

// 定义通信双方的信息结构体 
typedef struct {
	int type;
	char name[N];
	char data[256];
}MSG;

// ./server  192.168.3.196  10000
int main(int argc, const char *argv[])
{

	int sockfd;
	struct sockaddr_in  serveraddr;
	int n;
	MSG  msg;

	if(argc != 3)
	{
		printf("Usage:%s serverip  port.\n", argv[0]);
		return -1;
	}
    
    //创建socket套接字 
	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("fail to socket.\n");
		return -1;
	}

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));   //转换为整数后，转为网络字符 


    //链接 socket套接字和网络地址 
	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("fail to connect");
		return -1;
	}

	while(1)
	{
		printf("*****************************************************************\n");
		printf("* 1.register          2.login              3.quit               *\n");
		printf("*****************************************************************\n");
		printf("Please choose:");

		scanf("%d", &n);
		getchar();

		switch(n)
		{
		case 1:
			do_register(sockfd, &msg);
			break;
		case 2:
			if(do_login(sockfd, &msg) == 1)
			{
				goto next;
			}
			break;
		case 3:
			close(sockfd);
			exit(0);
			break;
		default:
			printf("Invalid data cmd.\n");
		}

	}

next:
	while(1)
	{
		printf("*****************************************************\n");
		printf("* 1.query_word   2.history_record   3.quit          *\n");
		printf("*****************************************************\n");
		printf("Please choose:");
		scanf("%d", &n);
		getchar();    //只接受一个字符 

		switch(n)
		{
			case 1:
				do_query(sockfd, &msg);
				break;
			case 2:
				do_history(sockfd, &msg);
				break;
			case 3:
				close(sockfd);
				exit(0);
				break;
			default :
				printf("Invalid data cmd.\n");
		}

	}
	
	return 0;
}



int  do_register(int sockfd, MSG *msg)
{
	msg->type = R;

	printf("Input name:");
	scanf("%s", msg->name);
	getchar();

	printf("Input passwd:");
	scanf("%s", msg->data);

	if(send(sockfd, msg, sizeof(MSG),0) < 0)
	{
		printf("fail to send.\n");
		return -1;
	}

	if(recv(sockfd, msg, sizeof(MSG), 0) < 0)
	{
		printf("Fail to recv.\n");
		return -1;
	}

	// ok !  or  usr alread exist.
	printf("%s\n", msg->data);

	return 0;
}

int do_login(int sockfd, MSG *msg)
{
	msg->type = L;

	printf("Input name:");
	scanf("%s", msg->name);
	getchar();

	printf("Input passwd:");
	scanf("%s", msg->data);

	if(send(sockfd, msg, sizeof(MSG),0) < 0)
	{
		printf("fail to send.\n");
		return -1;
	}

	if(recv(sockfd, msg, sizeof(MSG), 0) < 0)
	{
		printf("Fail to recv.\n");
		return -1;
	}

	if(strncmp(msg->data, "OK", 3) == 0)
	{
		printf("Login ok!\n");
		return 1;
	}
	else 
	{
		printf("%s\n", msg->data);
	}

	return 0;
}

int do_query(int sockfd, MSG *msg)
{
	msg->type = Q;
	puts("--------------");

	while(1)
	{
		printf("Input word:");
		scanf("%s", msg->data);
		getchar();

		//瀹㈡埛绔紝杈撳叆#鍙凤紝杩斿洖鍒颁笂涓�绾ц彍鍗�
		if(strncmp(msg->data, "#", 1) == 0)
			break;

		//灏嗚鏌ヨ鐨勫崟璇嶅彂閫佺粰鏈嶅姟鍣�
		if(send(sockfd,msg, sizeof(MSG), 0) < 0)
		{
			printf("Fail to send.\n");
			return -1;
		}

		//等待服务器传回来的单词注释信息 
		if(recv(sockfd, msg,sizeof(MSG), 0) < 0)
		{
			printf("Fail to recv.\n");
			return -1;
		}
		printf("%s\n", msg->data);
	}
		
	return 0;
}

int do_history(int sockfd, MSG *msg)
{

	msg->type = H;

	send(sockfd, msg, sizeof(MSG), 0);
	
	// 鎺ュ彈鏈嶅姟鍣紝浼犻�掑洖鏉ョ殑鍘嗗彶璁板綍淇℃伅
	while(1)
	{
		recv(sockfd, msg, sizeof(MSG), 0);

		if(msg->data[0] == '\0')
			break;

		//杈撳嚭鍘嗗彶璁板綍淇℃伅
		printf("%s\n", msg->data);
	}

	return 0;
}

