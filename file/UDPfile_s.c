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
        port_num = 49217;
    }
    else if (argc == 2){
        // for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
        port_num = atoi(argv[1]);
    }
    else  {
        printf("\nWrong input!\n");
        return -1;    
    }
    const int PORT = port_num;
    const int BUFF_SIZE = 512;

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
    char buf[BUFF_SIZE];
    memset(buf, 0, sizeof(buf));
    unsigned int flag = 0;
    int msg_len;
    int num = 0;
    char filename[100];
    memset(filename, 0, sizeof(filename));
    while(1){
        while(1){
            printf("receive filename\n");
            msg_len = recvfrom(sd, buf, BUFF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
            buf[msg_len] = 0;
//            printf("%s",buf);
            if(buf[0] != '\\' && buf[1] != '$'){
                
                for(int i=0; i<msg_len; i++)
			filename[i] = buf[i];
                break;
            }
        }
        FILE *fp = fopen(filename, "wb");
        fprintf(stdout, "filename: %s\n", filename);
        while(1){
            if((msg_len = recvfrom(sd, buf, BUFF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size)) <= 0){
//                printf("%d", msg_len);
                break;
                }
//            printf("%d\n",msg_len);
//            printf("%s\n", buf);
            if(buf[0] == '\\' && buf[1] == '$'){
//                fprintf(stdout, "%s\n", buf);
                 
                fwrite(buf, 1, sizeof(buf), fp);
            }
            else if(buf[0]=='\\' && buf[1] =='%' ) {
                 printf("file close\n");
                 fclose(fp);
                 break;
            }
            
            memset(buf, 0, sizeof(buf));
        }
    }

    // how bout closing?
    return 0;
}
