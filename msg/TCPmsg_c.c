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
    if (argc == 1){
        portNum = 20223;
        dstIP = "203.252.106.83"; // Byte
        printf("\nport number: %d, IPv4: %s\n", portNum, dstIP);
    }
    else if (argc == 3){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        portNum = atoi(argv[1]);
        dstIP = argv[2];
        printf("\nport number: %d, IPv4: %s\n", portNum, dstIP);
    }
    else  printf("\nWrong input!");

    const int YOUR_PORT = portNum;
    const char* DST_IP  = dstIP;


    //create a socket
    int sd;
    if ((sd = socket(PF_INET, SOCK_STREAM,0)) < 0){
        fprintf(stderr, "socket creating err!");
        exit(1);
    }

    //connect esatablish
    struct sockaddr_in dst_addr;
    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(YOUR_PORT);
    dst_addr.sin_addr.s_addr = inet_addr(DST_IP);

    if(connect(sd, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) == -1){
        close(sd);
        exit(1);
    }
    printf("connection established!\n");
    sleep(5);

    const int BUFF_SIZE = 512;
    unsigned int flag = 0;
    int msg_len;
    char* buf = NULL;
    buf = (char*) malloc(sizeof(char)* BUFF_SIZE);
    assert(buf != NULL);

    
    buf = "ABCDEFGHIJ";
    send(sd, buf, strlen(buf), flag);
    fprintf(stdout, "message sended: %s\n", buf);
    buf = "KLMNOPQRST";
    send(sd, buf, strlen(buf), flag);
    fprintf(stdout, "message sended: %s\n", buf);
    buf = "0123456789";
    send(sd, buf, strlen(buf), flag);
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
