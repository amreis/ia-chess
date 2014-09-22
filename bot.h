#ifndef BOT_H
#define BOT_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
	fprintf(stderr,"%s",msg);
	throw 42;
}

struct ServerBoard {
	bool fiftyMoves, end, whiteMoves;
	char board[64];
	int enPassant;

	ServerBoard()
	{
		fiftyMoves=false;
		end=false;
		whiteMoves=true;
		enPassant=-1;
	}

	void print()
	{
		printf("fifty_moves:%c\nend:%c\nwhite_moves:%c\nenpassant:%d\nboard:",
				fiftyMoves?'T':'F', end?'T':'F', whiteMoves?'T':'F', enPassant);
		for(int i=0;i<64;++i) printf("%c",board[i]);
		printf("\n");
	}
};

struct ConnectAPI{
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	ConnectAPI(int port, const char* name)
	{
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			error("ERROR opening socket\n");
		if((server = gethostbyname("localhost")) == NULL)
			error("ERROR, no such host\n");
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr,
			 (char *)&serv_addr.sin_addr.s_addr,
			 server->h_length);
		serv_addr.sin_port = htons(port);
		if((connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))) < 0)
			error("ERROR connecting\n");
		char myname[255] = "{\"name\": \"";
		strcat(myname, name);
		strcat(myname, "\"}");
		if(write(sockfd,myname,strlen(myname)) < 0) error("ERROR writing to socket");
	}

	~ConnectAPI() { close(sockfd); }

	ServerBoard readMsg()
	{
		char buffer[256];
		if(read(sockfd,buffer,255) < 0) error("ERROR reading from socket");

		printf("readMsg: %s\n",buffer);

		ServerBoard ret;

		int pt=9; if(buffer[pt] == 't'){ ret.end = true; return ret; }			//draw
		pt+=19;   if(buffer[pt] == 't'){ error("ERROR: Bad Move"); }			  //bad_move
		pt+=17;   memcpy(ret.board, buffer+pt, 64);							   //board
		pt+=102;  if(buffer[pt] == 't'){ ret.fiftyMoves = true; pt--; }		  //50moves
		pt+=17;   if(buffer[pt] != '0'){ ret.end = true; return ret; }			//winner
		pt+=16;   if(buffer[pt] != 'n'){ ret.enPassant = buffer[pt+4]-'0'; pt+=2; } //enpassant
		pt+=19;   if(buffer[pt] != '1'){ ret.whiteMoves = false; pt++; }		 //who_moves

		return ret;
	}

	void sendMove(int o1, int o2, int d1, int d2)
	{
		char buffer[] = "{\"to\": [0, 0], \"from\": [0, 0]}";
		buffer[8] += d1;
		buffer[11] += d2;
		buffer[24] += o1;
		buffer[27] += o2;

		printf("sendMove: %s (%d,%d,%d,%d)\n",buffer,o1,o2,d1,d2);

		if(write(sockfd,buffer,strlen(buffer)) < 0) error("ERROR writing to socket");
	}
};



#endif
