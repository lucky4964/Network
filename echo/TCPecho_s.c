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
        port_num = 30217;
        buff_size = 512; // Byte
        printf("\nport number: %d, buffer size: %d\n", port_num, buff_size);
    }
    else if (argc==3){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        port_num = atoi(argv[1]);
        buff_size = atoi(argv[2]);
        printf("\nport number: %d, buffer size: %d\n", port_num, buff_size);
    }
    else{
        printf("\nWrong input!\n");
        return -1;
    }
    const int PORT = port_num;
    const int BUFF_SIZE = buff_size;

    //create a socket
    int sd;
    if ((sd = socket(PF_INET, SOCK_STREAM,0)) < 0){ // return socket descriptor
        fprintf(stderr,"socket creating err!\n");
        exit(1);
    }
    
    
    //binding
    struct sockaddr_in my_addr, your_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    
    if(bind(sd, (struct sockaddr*)&my_addr, sizeof(my_addr))==-1 ){
        fprintf(stderr,"bind err\n");
        exit(1);
    }
    printf("bind complete!\n");
    printf("listening....\n");   
        // For unknown IP addr
        /*
        gethostbyname("DOMAIN")
        struct sockaddr_in, my_addr;
        memset(&my_addr, 0, sizeof(my_addr));
        my_addr.sin_family = AF_INET';
        my_addr.sin.port = htonl(MYPORT);
        printf("%s", inet_ntoa(my_addr.sin_addr));
        */

    unsigned int sin_size;
    int new_sd;
    //listen
    if(listen(sd, 5) == -1){
        fprintf(stderr,"listen() err\n");
        exit(1);
    }
    sin_size = sizeof(your_addr);
    printf("listend!\n");
    

    //connection accept return socket descriptor
    new_sd = accept(sd, (struct sockaddr *)&your_addr, &sin_size);
    if(new_sd == -1) {
        fprintf(stderr, "accept() err\n");
        exit(1);
    }
    printf("connection esatablished!\n");
    
    char buf[BUFF_SIZE];
    unsigned int flag = 0;
    int msg_len; 
    while((msg_len = recv(new_sd, buf, BUFF_SIZE, flag)) != 0){
        
        buf[msg_len] = 0;
        send(new_sd, buf, msg_len, flag);
        fprintf(stdout, "%s", buf);
    }
    
    // How bout old sd?
    //connection release
    close(new_sd);
    return 0;
}
