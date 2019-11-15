#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char* argv[] ){

    //CLI
    int portNum;
    char* dstIP;
    char* filename;

    if (argc == 1){
        portNum = 49217;
        dstIP = "203.252.106.83"; // Byte
        filename = "hello.txt";
        printf("\nport number: %d, IPv4: %s, Filename: %s\n", portNum, dstIP, filename);
    }
    else if (argc == 4){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        portNum = atoi(argv[1]);
        dstIP = argv[2];
        filename = argv[3];
        printf("\nport number: %d, IPv4: %s, Filename: %s\n", portNum, dstIP, filename);
    }
    else  printf("\nWrong input!\n");

    const int PORT = portNum;
    const char* DST_IP  = dstIP;
    const char* FILENAME = filename;


    //create a socket
    int sd;
    if ((sd = socket(PF_INET, SOCK_DGRAM,0)) == -1){
        fprintf(stderr, "socket creating err!");
        exit(1);
    }

    printf("socket created!\n");

    struct sockaddr_in serv_addr, clnt_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(DST_IP);




    char msg1[] = "ABCDEFGHIJ";
    char msg2[] = "KLMNOPQRST";
    char msg3[] = "0123456789";
    unsigned int flag = 0, addr_size;
    int msg_len;
    const int BUFF_SIZE = 30;
    char* buf = (char*)malloc(sizeof(char) * BUFF_SIZE);
    assert(buf != NULL);
    
    //send message
    unsigned int flag = 0;
    int msg_len;
    char* buf = NULL;
    buf = (char*) malloc(sizeof(char)* BUFF_SIZE);
    assert(buf != NULL);

    
    buf = "ABCDEFGHIJ";
    sendto(sd, buf, strlen(buf), flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    fprintf(stdout, "message sended: %s\n", buf);
    buf = "KLMNOPQRST";
    sendto(sd, buf, strlen(buf), flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    fprintf(stdout, "message sended: %s\n", buf);
    buf = "0123456789";
    sendto(sd, buf, strlen(buf), flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    fprintf(stdout, "message sended: %s\n", buf);
    

    //send message
    while(1){
        buf = (char*)malloc(sizeof(char) * BUFF_SIZE);
        assert(buf != NULL);
        fprintf(stdout, "Input message to send: ");
        fgets(buf,BUFF_SIZE, stdin);

        if(!strcmp(buf, "q\n")) break;
        send(sd, buf, strlen(buf), flag);
        free(buf);
    }

    //connection release
    close(sd);
    return 0;
}
