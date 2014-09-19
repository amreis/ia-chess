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

struct liacBoard{
    //bad_move, black_infractions, white_infractions
    bool fifty_moves, end, white_moves;
    char board[64];
    int enpassant; //null = -1
    
    liacBoard()
    {
        fifty_moves=false;
        end=false;
        white_moves=true;
        enpassant=-1;
    }
    
    void print()
    {
        printf("fifty_moves:%c\nend:%c\nwhite_moves:%c\nenpassant:%d\nboard:",
                fifty_moves?'T':'F', end?'T':'F', white_moves?'T':'F', enpassant);
        for(int i=0;i<64;++i) printf("%c",board[i]);
        printf("\n");
    }
};

struct liacAPI{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    liacAPI(int port)
    {
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("ERROR opening socket\n");
        if((server = gethostbyname("localhost")) == NULL) error("ERROR, no such host\n");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(port);
        if((connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))) < 0) error("ERROR connecting\n");
        char name[] = "{\"name\": \"Andrew Martin\"}";
        if(write(sockfd,name,strlen(name)) < 0) error("ERROR writing to socket");
    }
    
    ~liacAPI() { close(sockfd); }
    
    liacBoard readMsg()
    {
        char buffer[256];
        if(read(sockfd,buffer,255) < 0) error("ERROR reading from socket");
        
		printf("readMsg: %s\n",buffer);
		
        liacBoard ret;
        
        int pt=9; if(buffer[pt] == 't'){ ret.end = true; return ret; }            //draw
        pt+=19;   if(buffer[pt] == 't'){ error("ERROR: Bad Move"); }              //bad_move
        pt+=17;   memcpy(ret.board, buffer+pt, 64);                               //board
        pt+=102;  if(buffer[pt] == 't'){ ret.fifty_moves = true; pt--; }          //50moves
        pt+=17;   if(buffer[pt] != '0'){ ret.end = true; return ret; }            //winner
        pt+=16;   if(buffer[pt] != 'n'){ ret.enpassant = buffer[pt+4]-'0'; pt+=2; } //enpassant
        pt+=19;   if(buffer[pt] != '1'){ ret.white_moves = false; pt++; }         //who_moves
        
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

int main(int argc, char *argv[])
{
    liacAPI sock(50200);
    liacBoard curb = sock.readMsg();
    curb.print();
    sock.sendMove(6,3,4,3);
    curb = sock.readMsg();
    curb.print();
    
    return 0;
}

