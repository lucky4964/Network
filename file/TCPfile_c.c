#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>

const int BUFF_SIZE = 512;
const char* _EOF_ = "$EOF";
const char* _FIN_ = "$FIN";
int main(int argc, char* argv[] ){

    
    //CLI
    int portNum;
    char* dstIP;
    char* filename;

    if (argc == 1){
        portNum = 49217;
        dstIP = "203.252.106.83"; // Byte
//        dstIP = "203.252.106.83";
        filename = "hello.txt";
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
    
    //create a socket
    int sd;
    if ((sd = socket(PF_INET, SOCK_STREAM,0)) < 0){ // return socket descriptor
        fprintf(stderr,"socket creating err!\n");
        exit(1);
    }
    
    
    //connect
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(DST_IP);

    
    if(connect(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        printf("connect err!\n");
        close(sd);
        exit(1);
    }
    printf("connection established!\n");
    

    const int BUFF_SIZE = 256;


    FILE* fp;
    char* choice;
    
    int filesize, fpsize, sendsize, msg_len;
//    char* buf = NULL;
//    buf = (char*) malloc(sizeof(char)* BUFF_SIZE);
//    assert(buf != NULL);

    char buf[BUFF_SIZE];
    memset(buf, 0, sizeof(buf));

    char fdata[100];
    memset(fdata, 0, sizeof(fdata));

    buf[0] ='\\';
    strcat(buf, FILENAME);
    printf("filename: %s\n", buf);

    sendsize = 0;
            
        // open file
    fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("fopen() error!");
        close(sd);
        exit(1);
    }
    printf("fileread... \n");

        //check the file size;
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    sprintf(fdata, "%d", filesize);
    printf("filesieze: %s\n",fdata);
    buf[strlen(buf)] ='\\';
    strcat(buf, fdata);
    buf[strlen(buf)] = '\\';
    printf("fsize packet: %s\n",buf);


        // send file size
    send(sd, buf, sizeof(buf), 0);

        // read and send
    while (sendsize != filesize) {
        fpsize = fread(buf, 1, BUFF_SIZE, fp);
        sendsize = sendsize + fpsize;                
        msg_len = send(sd, buf, fpsize, 0);
//        printf("%d\n", msg_len);
    }
    printf("File Transfer Complete!\n");
    fclose(fp);
    close(sd);

    return 0;
}
