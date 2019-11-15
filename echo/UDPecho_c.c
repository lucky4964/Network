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
        portNum = 49494;
        dstIP = "203.252.106.83"; // Byte
        printf("\nport number: %d, IPv4: %s\n", portNum, dstIP);
    }
    else if (argc == 3){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        portNum = atoi(argv[1]);
        dstIP = argv[2];
        printf("\nport number: %d, IPv4: %s\n", portNum, dstIP);
    }
    else  printf("\nWrong input!\n");

    const int YOUR_PORT = portNum;
    const char* DST_IP  = dstIP;


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
    serv_addr.sin_port = htons(YOUR_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(DST_IP);


    char msg1[] = "Good";
    char msg2[] = "Evening";
    char msg3[] = "Everybody";
    unsigned int flag = 0, addr_size;
    int msg_len;
    const int BUFF_SIZE = 30;
    char* buf = (char*)malloc(sizeof(char) * BUFF_SIZE);
    assert(buf != NULL);
    
    //send message
    sendto(sd, msg1, strlen(msg1), flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    sendto(sd, msg2, strlen(msg2), flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    sendto(sd, msg3, strlen(msg3), flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    printf("messages are sended !\n");
    for(int i=0; i<3; i++){
        printf("waiting %dth message...\n\n", i);
        addr_size = sizeof(clnt_addr);
        msg_len = recvfrom(sd, buf, BUFF_SIZE, flag, (struct sockaddr*)&clnt_addr, &addr_size);
        buf[msg_len] = 0;
        fprintf(stdout, "%dth message received from server: %s\n", i, buf);
    }

    //connection release
    close(sd);
    return 0;
}
