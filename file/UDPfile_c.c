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
        filename = "hello.good";
        
    }
    else if (argc == 4){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        portNum = atoi(argv[2]);
        dstIP = argv[1];
        filename = argv[3];
        
    }
    else  printf("\nWrong input!\n");

    const int PORT = portNum;
    const char* DST_IP  = dstIP;
    const char* FILENAME = filename;
    printf("\nport number: %d, IPv4: %s, Filename: %s\n", PORT, DST_IP, FILENAME);
    sleep(2);
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


    //read file
    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("fopen() error!");
        close(sd);
        exit(1);
    }
    printf("file read!\n\n");


    unsigned int flag = 0, addr_size;
    int msg_len;
    const int BUFF_SIZE = 100;
    int fpsize = 0;
    
    char buf[BUFF_SIZE];
    memset(buf, 0, sizeof(buf));

    
    strcpy(buf, FILENAME);
    for(int i = 0; i < 5; i++)
        sendto(sd, buf, strlen(buf), flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    fprintf(stdout, "message sended: %s\n", buf);
    sleep(1);   

    //send message
    while(1){
        if ((fpsize = fread(buf, 1, BUFF_SIZE, fp)) == 0) break;

        char send_buf[BUFF_SIZE+2];
        memset(send_buf, 0, sizeof(send_buf));
        send_buf[0] = '\\';
        send_buf[1] = '$';
//        printf("%s -> send_buf%d\n", send_buf, sizeof(send_buf));
        strcat(send_buf, buf);
        
        send_buf[BUFF_SIZE+2] = 0;
        msg_len = sendto(sd, send_buf, strlen(send_buf), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//        printf("%s\n", send_buf);
//        printf("%d\n", msg_len);
        memset(buf, 0, sizeof(buf));
    }
    for(int i=0; i<5; i++){
         buf[0] = '\\';
         buf[1] = '%';
         
         sendto(sd, buf, 2, flag, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    }
    //connection release
//    printf("%s\n",DST_IP);
    close(sd);
    return 0;
}
