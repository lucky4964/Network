#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char* argv[] ){


    //CLI
    int port_num, buff_size;
    if (argc == 1){
        port_num = 49494;
        buff_size = 512; // Byte
    }
    else if (argc == 3){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        port_num = atoi(argv[1]);
        buff_size = atoi(argv[2]);
    }
    else  {
        printf("\nWrong input!\n");
        return -1;    
    }
    const int PORT = port_num;
    const int BUFF_SIZE = buff_size;

    printf("\nport number: %d, buffer size: %d\n", PORT, BUFF_SIZE);

    //create a socket
    int sd;
    if ((sd = socket(PF_INET, SOCK_DGRAM,0)) < 0){ // return socket descriptor
        fprintf(stderr,"socket creating err!\n");
        exit(1);
    }

    //binding
    struct sockaddr_in serv_addr, clnt_addr;
    int clnt_addr_size;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    //int opt = 1;
    //setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int bind_err = bind(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(bind_err == -1){
        fprintf(stderr, "bind err\n");
        exit(1);
    }


    sleep(5); // TCP 랑 차이를 보기 위해서
    printf("Destination Port number: %d\n", PORT);

        // For unknown IP addr
        /*
        gethostbyname("DOMAIN")
        struct sockaddr_in, my_addr;
        memset(&my_addr, 0, sizeof(my_addr));
        my_addr.sin_family = AF_INET';
        my_addr.sin.port = htonl(MYPORT);
        printf("%s", inet_ntoa(my_addr.sin_addr));
        */
    
    printf("bind complete!\n");
    char* buf = (char*)malloc(sizeof(char) * BUFF_SIZE);
    assert(buf != NULL);
    unsigned int flag = 0;
    int msg_len;
    int num = 0;

    while((msg_len = recvfrom(sd, buf, BUFF_SIZE, flag, (struct sockaddr*)&clnt_addr, &clnt_addr_size)) != 0){
        buf[msg_len] = 0;
        fprintf(stdout, "%s\n", buf);
    }

    // how bout closing?
    return 0;
}
