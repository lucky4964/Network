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
    int port_num;
    if (argc == 1){
        port_num = 49217;
        printf("\nport number: %d\n", port_num);
    }
    else if (argc==2){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        port_num = atoi(argv[1]);
        printf("\nport number: %d\n", port_num);
    }
    else{
        printf("\nWrong input!\n");
        return -1;
    }
    const int PORT = port_num;

    //create a socket
    int sd;
    if ((sd = socket(PF_INET, SOCK_STREAM,0)) < 0){ // return socket descriptor
        fprintf(stderr,"socket creating err!\n");
        exit(1);
    }
    
    
    //binding
    struct sockaddr_in serv_addr, clnt_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    
    if(bind(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1 ){
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


    //listen
    if(listen(sd, 5) == -1){
        fprintf(stderr,"listen() err\n");
        exit(1);
    }
    printf("listend!!\n");

    
//    char* buf = (char*)malloc(sizeof(char)* BUFF_SIZE);
//    assert(buf != NULL);
    char buf[BUFF_SIZE];


    unsigned int flag = 0;
    int msg_len; 
    FILE *fp =NULL;
    
    
 // Iterative server
    int pkt_num = 0;
    while (1) {
        int filesize;  
        int bufsize;
        int recvbyte;
        int filename = 0;
        char fdata[100];
        memset(fdata, 0, sizeof(fdata));
        memset(buf, 0, sizeof(buf));
	
        printf("\n\nwaiting connection...\n\n");
        // accept - return new socket descriptor(>0)
        //printf("wait accepting clnt_sock...\n");
        unsigned int sin_size;
        int new_sd;
        sin_size = sizeof(clnt_addr);

        new_sd = accept(sd, (struct sockaddr *)&clnt_addr, &sin_size);
        if(new_sd == -1) {
            fprintf(stderr, "accept() err\n");
            close(new_sd);
            exit(1);
        }
        printf("connection esatablished!\n");
        
            //printf("file?\n");

            //get file name
            if (filename == 0) {
                bufsize = 1;
                filename = recv(new_sd, buf, bufsize, flag);
                int i = 0;
                while(i < 3){
                    if(buf[0]=='\\') i++;
//                    printf("%c\n", buf[0]);
                    filename = recv(new_sd, buf, bufsize, flag);
                    strcat(fdata, buf);

                }
//                printf("first packet: %s\n",fdata);
                char *ptr = strtok(fdata, "\\");
//                printf("filename: %s\n", fdata);
                fp = fopen(ptr, "wb");
                ptr = strtok(NULL, "\\");
                filesize = atoi(ptr);
//                printf("filesize:%d\n", filesize);
                ptr = strtok(NULL, "\\");
                filename = 100;

                printf("Filename: %s, Filesize: %d\n", fdata, filesize);                     
            }    
            

//            printf("recieved... ");
             //receive file
            bufsize = BUFF_SIZE;
            recvbyte = 0;
            while (filesize != recvbyte) {
                if (filesize < BUFF_SIZE)
                    bufsize = filesize;
                recvbyte = recv(new_sd, buf, bufsize, 0);
                filesize = filesize - recvbyte;
//                printf("%d\n", filesize);
                fwrite(buf, 1, recvbyte, fp);
                pkt_num++;
            }
            
            printf("finish to write!\n");
            fclose(fp);
            filename = 0;
            
        close(new_sd);
    }

    return 0;
}
