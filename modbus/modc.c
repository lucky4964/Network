#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

const int MAX_BUF = 100;

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


void readCoil(int *sock, int transID){

    unsigned short address, coilNum;
    printf("Start address: ");
    scanf("%hd", &address);
    printf("Number of coils: ");
    scanf("%hd", &coilNum);


    
    /* build MODBUS request */
    unsigned char obuf[MAX_BUF];
    // char data[MAX_BUF];
    memset(obuf, 0, sizeof(obuf));
    obuf[0] = transID >> 8;
    obuf[1] = transID & 0xff;
    // length
    obuf[4] = 0x00;
    obuf[5] = 0x06;
    obuf[6] = 0x01;
    //func code
    obuf[7] = 0x01;
    obuf[8] = address >> 8;
    obuf[9] = address & 0xff;
    obuf[10] = coilNum >> 8;
    obuf[11] = coilNum & 0xff;

    // obuf[0] = 0x01;
    // obuf[1] = address >> 8;
    // obuf[2] = address & 0xff;
    // obuf[3] = coilNum >> 8;
    // obuf[4] = coilNum & 0xff;


    /* send request */
    int msg_len = send(*sock, obuf, 12, 0);
    for (int i =0; i< 12; i++){
        printf("%.2x", obuf[i]);
        if(i % 2 == 1) printf(" ");
    }
        
    char ibuf[MAX_BUF];
    memset(ibuf, 0, sizeof(ibuf));
    /* receive and analyze response */
    msg_len = recv(*sock, ibuf, sizeof(ibuf), 0);

    unsigned short byteCount = ibuf[8];
    if (msg_len < 9)
        if (msg_len == 0)
            printf("unexpected close of connection at remote end\n");
        else
            printf("response was too short - %d chars\n", msg_len);
    else if (ibuf[7] & 0x80)
        printf("MODBUS exception response - type %d\n", ibuf[8]);
    else{
        printf("\n\n");
        for (int i =0; i< 7; i++){
            printf("%.2x", ibuf[i]);
            if(i % 2 == 1) printf(" ");
        }
        for(int i=0; i<byteCount; i++)
            printf("\ndata: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(ibuf[9+i]));
        printf("\n");
        // int temp = coilNum;
        // for (int i=byteCount; i<1; i++){
        //     // unsigned short w = (ibuf[9 + i + i] << 8) + ibuf[10 + i + i];
        //     unsigned short w = ibuf[9+i];
        //     int remain = ( temp < 8)? temp : 8;
        //     for (int j=0; j<remain; j++ )
        //     {
        //         printf("coil %d = %d\n", (byteCount-i)*8 + j + address, (w>>j) & (0x01));
        //     }
        //     temp -= 8;
        // }
    }
    return;
}
void writeCoil(int *sock, int transID){

    unsigned short address, coilNum;

    int data;
    printf("Start address: ");
    scanf("%hd", &address);
    printf("Number of coils: ");
    scanf("%hd", &coilNum);
    unsigned short byteCount = (coilNum+7)/8 ; // coilNum > 0
    unsigned short* bitData  = (unsigned short*)malloc(sizeof(unsigned short) * byteCount);
    for(int i =0; i< byteCount; i++ ){
        printf("%dth 8bit Data to be written (in Hexadecimal format, 00 ~ ff): ", i + 1);
        scanf("%hx", &bitData[i]);
    }
    
    // printf("\nbyte count: %x, %d\n", bitData[0], sizeof(unsigned short));
    unsigned int length = 12 + byteCount;
    // 

    // for(int i =0; i<byteCount; i++)
    //     bitData[i] = data >> ((byteCount-1 - i) * 8);

    /* build MODBUS request */
    unsigned char obuf[MAX_BUF];
    memset(obuf, 0, sizeof(obuf));
    obuf[0] = transID >> 8;
    obuf[1] = transID & 0xff;
    // length
    obuf[4] = 0;
    obuf[5] = length;
    obuf[6] = 0x01;
    //func code
    obuf[7] = 0x0f;
    obuf[8] = address >> 8;
    obuf[9] = address & 0xff;
    obuf[10] = coilNum >> 8;
    obuf[11] = coilNum & 0xff;
    //byte count
    obuf[12] = byteCount;
    for(int i= 0; i < byteCount; i++){
        obuf[13+ i] = bitData[byteCount - 1 - i] & 0xff;
    }

    /* send request */
    int msg_len = send(*sock, obuf, 13 + byteCount, 0);
    for (int i = 0; i< length; i++){
        printf("%.2x", obuf[i]);
        if(i % 2 == 1) printf(" ");
    }
    for(int i=0; i<byteCount; i++)
        printf("\ndata: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(obuf[13+i]));

    char ibuf[MAX_BUF];
    memset(ibuf, 0, sizeof(ibuf));
    /* receive and analyze response */
    msg_len = recv(*sock, ibuf, sizeof(ibuf), 0);


    if (msg_len < 9)
        if (msg_len == 0)
            printf("unexpected close of connection at remote end\n");
        else
            printf("response was too short - %d\n", msg_len);
    else if (ibuf[7] & 0x80)
        printf("MODBUS exception response - type %d\n", ibuf[8]);
    // else if (i != (9+2*num_regs))
    // {
    //     printf("incorrect response size is %d expected %d\n",i,(9+2*num_regs));
    // }
    else{
        printf("\n\n");
        for (int i =0; i< msg_len; i++){
            printf("%.2x", ibuf[i]);
            if(i % 2 == 1) printf(" ");
        }
        printf("\n");
    }
}
void readReg(int *sock, int transID){

    unsigned short address, regNum;
    printf("Start address: ");
    scanf("%hd", &address);
    printf("Number of coils: ");
    scanf("%hd", &regNum);


    
    /* build MODBUS request */
    unsigned char obuf[MAX_BUF];
    // char data[MAX_BUF];
    memset(obuf, 0, sizeof(obuf));
    obuf[0] = transID >> 8;
    obuf[1] = transID & 0xff;
    // length
    obuf[4] = 0x00;
    obuf[5] = 0x06;
    obuf[6] = 0x01;
    //func code
    obuf[7] = 0x03;
    obuf[8] = address >> 8;
    obuf[9] = address & 0xff;
    obuf[10] = regNum >> 8;
    obuf[11] = regNum & 0xff;

    /* send request */
    int msg_len = send(*sock, obuf, 12, 0);
    for (int i =0; i< 12; i++){
        printf("%.2x", obuf[i]);
        if(i % 2 == 1) printf(" ");
    }
        
    char ibuf[MAX_BUF];
    memset(ibuf, 0, sizeof(ibuf));
    /* receive and analyze response */
    msg_len = recv(*sock, ibuf, sizeof(ibuf), 0);
    printf("\n\n");
    for (int i =0; i< 12; i++){
        printf("%.2x", ibuf[i]);
        if(i % 2 == 1) printf(" ");
    }
    printf("\n");

    if (msg_len < 9)
        if (msg_len == 0)
            printf("unexpected close of connection at remote end\n");
        else
            printf("response was too short - %d chars\n", msg_len);
    else if (ibuf[7] & 0x80)
        printf("MODBUS exception response - type %d\n", ibuf[8]);
    else if (msg_len != (9+2*regNum))
        printf("incorrect response size is %d expected %d\n",msg_len ,(9+2*regNum));
    else
        for (int i=0; i<regNum; i++)
        {
            unsigned short w = (ibuf[9 + i+ i] << 8) + ibuf[ 10 + i + i];
            printf("word %d = %d\n", i, w);
        }

    /* close down connection */
    close(*sock);

}
void writeReg(int *sock, int transID){

    unsigned short address, regNum;

    int data;
    printf("Start address: ");
    scanf("%hd", &address);
    printf("Number of coils: ");
    scanf("%hd", &regNum);
    unsigned short byteCount = regNum * 2 ; // coilNum > 0
    unsigned short* bitData  = (unsigned short*)malloc(sizeof(unsigned short) * regNum);
    for(int i =0; i< regNum; i++ ){
        printf("Data to be written: ");
        scanf("%hd", &bitData[i]);
    }
    
    // printf("\nbyte count: %x, %d\n", bitData[0], sizeof(unsigned short));
    unsigned int length = 13 + byteCount;
    // 

    // for(int i =0; i<byteCount; i++)
    //     bitData[i] = data >> ((byteCount-1 - i) * 8);

    /* build MODBUS request */
    unsigned char obuf[MAX_BUF];
    memset(obuf, 0, sizeof(obuf));
    obuf[0] = transID >> 8;
    obuf[1] = transID & 0xff;
    // length
    obuf[4] = 0;
    obuf[5] = length;
    obuf[6] = 0x01;
    //func code
    obuf[7] = 0x10;
    obuf[8] = address >> 8;
    obuf[9] = address & 0xff;
    obuf[10] = regNum >> 8;
    obuf[11] = regNum & 0xff;
    //byte count
    obuf[12] = byteCount;
    for(int i= 0; i < byteCount; i ++){
        obuf[14+ 2*i -1] = bitData[i] >> 8;
        obuf[14+ 2*i] = bitData[i] & 0xff;
    }

    /* send request */
    int msg_len = send(*sock, obuf, 14 + byteCount, 0);
    for (int i = 0; i< length; i++){
        printf("%.2x", obuf[i]);
        if(i % 2 == 1) printf(" ");
    }

    char ibuf[MAX_BUF];
    memset(ibuf, 0, sizeof(ibuf));
    /* receive and analyze response */
    msg_len = recv(*sock, ibuf, sizeof(ibuf), 0);


    if (msg_len < 9)
        if (msg_len == 0)
            printf("unexpected close of connection at remote end\n");
        else
            printf("response was too short - %d\n", msg_len);
    else if (ibuf[7] & 0x80)
        printf("MODBUS exception response - type %d\n", ibuf[8]);
    // else if (i != (9+2*num_regs))
    // {
    //     printf("incorrect response size is %d expected %d\n",i,(9+2*num_regs));
    // }
    else{
        printf("\n\n");
        for (int i =0; i< msg_len; i++){
            printf("%.2x", ibuf[i]);
            if(i % 2 == 1) printf(" ");
        }
        printf("\n");
    }
}



int main(int argc, char* argv[] ){


    //CLI
    int portNum;
    char* dstIP;
    int transID = 0;
    if (argc == 1){
        portNum = 502;
        dstIP = "127.0.0.1"; // Byte
        printf("\nport number: %d, IPv4: %s\n", portNum, dstIP);
    }
    else if (argc == 2){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        portNum = 502;
        dstIP = argv[1];
        printf("\nport number: %d, IPv4: %s\n", portNum, dstIP);
    }
    else  printf("\nWrong input!");

    const int YOUR_PORT = portNum;
    const char* DST_IP  = dstIP;



    char option_char;

    do{
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



        const int BUFF_SIZE = 512;
        unsigned int flag = 0;
        int msg_len;
        if(connect(sd, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) == -1){
            close(sd);
            exit(1);
        }
        printf("connection established!\n");


        printf("\n\tWhich option will you do? \n"
			"\ta - read coil\n"
			"\ts - Write multiple coils\n"
			"\td - Read Holding registers\n"
			"\tf - Write multiple (holding) registers\n");
		printf("\tCommand(q to quit): ");
        option_char = getchar();

        switch(option_char){
            
            case 'a':
                readCoil(&sd, transID++);
                break;

            case 's':
                writeCoil(&sd, transID++);
                break;

            case 'd':
                readReg(&sd, transID++);
                break;

            case 'f':
                writeReg(&sd, transID++);
                break;

            default: break;

        }
        getchar();
        close(sd);
    }while(option_char != 'q');

    return 0;
}



