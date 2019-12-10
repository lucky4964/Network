#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;
const int SMALL_BUF = 100;

// void request_handling(int* sock);
void send_err(int* fp);
void send_data(int* fp, char *ct, char *file_name);
// void update_data(FILE* fp, char *body, int* sock);
char* content_type(char *file);

int main (int argc, char *argv[]){

    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    unsigned int clnt_adr_size;
    char buf[BUF_SIZE];
    memset(buf, 0, sizeof(buf));

    // CLI
    int port = 0;
    if(argc == 1){
        port = 49172;
    }
    else if(argc == 2){
        port = atoi(argv[1]); 
    }
    else{
        printf("wrong input!\n");
        exit(1);
    }
    const int PORT = port;
    printf("PORT NUMBER: %d\n", PORT);

    // create socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    //binding
    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) ==-1 ){
        printf("Binding error!\n");
        exit(1);
    }

    //listen
    if(listen(serv_sock, 20) == -1){
        printf("Listen error!\n");
        exit(1);
    }
    

    

    while(1){
        clnt_adr_size = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
        printf("Connection Request: %s:%d\n",
                            inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));


        char method[10];
        char ct[15];
        char file_name[30];
        

        
        int len = 0;
        len = recv(clnt_sock, buf, sizeof(buf), 0);
        // while(() != 0){
        //     strcat(buf, req_line);
        // }
        printf("%s", buf);
        char req_line[BUF_SIZE];
        memset(req_line, 0, sizeof(req_line));
        strcpy(req_line, buf);
        
        
        if(strstr(req_line, "HTTP/") == NULL){
            send_err(&clnt_sock);
            close(clnt_sock);
            exit(1);
        }
        strcpy(method, strtok(req_line, " /"));
        strcpy(file_name, strtok(NULL, " /"));
        printf("method: %s\nfile_name: %s\n", method, file_name);

        //no filename case
        if(strstr(file_name, "HTTP") != NULL){
            memset(file_name, 0, sizeof(file_name));
            char* temp = "Index.html";
            strcpy(file_name, temp);
        }
        strcpy(ct, content_type(file_name));

        if(!strcmp(method, "GET")){
            printf("GET\n");
            send_data(&clnt_sock, ct, file_name);
        }  
        else if(!strcmp(method, "POST")){
            
            printf("fucking POST\n");

        }
        else{
            printf("fucking nope");
        }

        // request_handling(&clnt_sock);
        close(clnt_sock);

    }
    close(serv_sock);
    return 0;
}

// // Warning Typo err! 
void send_data(int *fp, char *ct, char *file_name){

    FILE* send_file;
    char buf[BUF_SIZE];
    memset(buf, 0, sizeof(buf));
    char cnt_type[SMALL_BUF];
    sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
    printf("file: %s...\n", file_name);

    send_file = fopen(file_name, "r");
    if(send_file == NULL){
        send_err(fp);
        return ;
    }

    int byte;
    char filesize[SMALL_BUF];
    memset(filesize, 0, sizeof(filesize));
    fseek(send_file, 0, SEEK_END);
    byte = ftell(send_file);
    fseek(send_file, 0, SEEK_SET);

    sprintf(filesize, "%d", byte);
    printf("filesize: %s\n", filesize);

    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
   
    char cnt_len[BUF_SIZE];
    memset(cnt_len, 0, sizeof(cnt_len));
    strcat(cnt_len,"Content-length:");
    strcat(cnt_len,filesize);
    strcat(cnt_len,"\r\n");

    fputs(protocol, fp);
    // send(*fp, protocol, SMALL_BUF, 0);
    // printf("%s",protocol);

    // fputs(server, fp);
    send(*fp, server, SMALL_BUF, 0);
    // printf("%s",server);

    // fputs(cnt_len, fp);
    send(*fp, cnt_len, SMALL_BUF, 0);
    // printf("%s",cnt_len);

    // fputs(cnt_type, fp);
    send(*fp, cnt_type, SMALL_BUF, 0);
    // printf("%s",cnt_type);

    while(fgets(buf, BUF_SIZE, send_file) != NULL){
        // send(buf, fp);
        send(*fp, buf, SMALL_BUF, 0);
        // printf("%s\n",buf);
        memset(buf, 0, sizeof(buf));
    }
    // printf("\nsending complete!\n");


    // fflush(fp);
    
}

// void update_data(FILE* fp, char *body, int* sock){

//     int serv_sock, clnt_sock;
//     struct sockaddr_in serv_adr, clnt_adr;
//     unsigned int clnt_adr_size;
//     char buf[BUF_SIZE];

//     FILE* send_file;
//     char cnt_type[] = "Content-type:text/html\r\n\r\n";
//     char protocol[] = "HTTP/1.0 200 OK\r\n";
//     char server[] = "Server:Linux Web Server \r\n";
   
//     close(sock);
//     clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);

//     int byte = strlen(body);
//     char filesize[SMALL_BUF];
//     memset(filesize, 0, sizeof(filesize));
//     sprintf(filesize, "%d", byte);
//     printf("\n\n%s\n\n",filesize);

//     char cnt_len[BUF_SIZE];
//     memset(cnt_len, 0, sizeof(cnt_len));
//     strcat(cnt_len,"Content-length:");
//     strcat(cnt_len,filesize);
//     strcat(cnt_len,"\r\n");

//     fputs(protocol, fp);
//     // printf("%s",protocol);

//     fputs(server, fp);
//     // printf("%s",server);

//     fputs(cnt_len, fp);
//     // printf("%s",cnt_len);

//     fputs(cnt_type, fp);
//     // printf("%s",cnt_type);

//     fputs(body, fp);
//     fflush(fp);
//     fclose(fp);
    
// }


char* content_type(char* file){

    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));
    
    if(!(strcmp(extension, "html")) || !(strcmp(extension, "htm")))
        return "text/html";
    else
        return "text/plain";

}

void send_err(int* fp){

    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Serer:Linux Web Server \r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";

    // char* cnt_len = "Content-length:0\r\n";

    // fputs(protocol, fp);
    send(*fp, protocol, SMALL_BUF, 0);
    printf("%s",protocol);

    // fputs(server, fp);
    send(*fp, server, SMALL_BUF, 0);
    printf("%s",server);

    // fputs(cnt_len, fp);
    // send(*fp, cnt_len, SMALL_BUF, 0);
    // printf("%s",cnt_len);

    // fputs(cnt_type, fp);
    send(*fp, cnt_type, SMALL_BUF, 0);
    printf("%s",cnt_type);
}
