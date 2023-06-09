#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <netinet/in.h>

#define SERVERPORT 8888
#define MAXBUF 1024

//소켓 생성 => server와 connect() =>write()
//=> 소켓 shutdown() => file open() => read(), write()


int main(int argc, char* argv[]){
	int sockd;
	int counter;
	int fd;
	struct sockaddr_in xferServer; //client는 구조체 1개만!
	char buf[MAXBUF];
	int returnStatus;

	if(argc<3){
		fprintf(stderr, "Usage: %s <ip address> <filename> [dest filename]\n", argv[0]);
		exit(1);
	}

/*Create a socket*/
	sockd=socket(AF_INET,SOCK_STREAM,0);
	if(sockd==-1){
		fprintf(stderr, "Could not create socket!\n");
		exit(1);
	}
/*set up server info*/
	xferServer.sin_family=AF_INET;
	xferServer.sin_addr.s_addr=inet_addr(argv[1]);
	xferServer.sin_port=htons(SERVERPORT);
/*connect to the server*/
	returnStatus=connect(sockd,(struct sockaddr*)&xferServer,
						sizeof(xferServer)); //소켓, 구조체, 구조체 크기
	if (returnStatus == -1){
		fprintf(stderr, "Could not connect to server!\n");
		exit(1);
	}
/*send filename to server*/
	returnStatus=write(sockd,argv[2],strlen(argv[2])+1); //소켓, 파일이름, 파일이름 크기 +1
	if (returnStatus == -1){
		fprintf(stderr, "Could send filename to server!\n");
		exit(1);
	}
/*call the shutdown to set socket to read only*/
	shutdown(sockd, SHUT_WR);
/*open up a handle to destination file to recv contents*/
	//from the server
	fd=open(argv[3], O_WRONLY | O_CREAT |O_APPEND);
	
	if (fd == -1){
		fprintf(stderr, "Could not open destination file!\n");
		exit(1);
	}
/*read the file from socket*/
	while((counter=read(sockd, buf, MAXBUF))>0){
		//send contents to stdout
		write(fd,buf,counter);
	}
	if(counter==-1){
		fprintf(stderr, "Could not read file from socket!\n");
	}
	close(sockd);
	return 0;
}