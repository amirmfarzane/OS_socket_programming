#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>

#define sever_port_val 7094

int connectServer(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { // checking for errors
        printf("Error in connecting to server\n");
    }

    return fd;
}

#define BUFFER_SIZE 1024
int main(int argc, char const *argv[]) {
    int port = atoi(argv[1]);
    int udp_sock, broadcast = 1, opt = 1;

    //inter name:
    char* enter_name = "Please enter your username:";
    write(1 , enter_name , strlen(enter_name));
    char name[BUFFER_SIZE] =  {0};
    
    int name_bytes = read(0 , name , BUFFER_SIZE);
    name[name_bytes - 1] = '\0';

    char welcome[BUFFER_SIZE];
    strcpy(welcome , "welcome ");
    strcat(welcome , name );
    strcat(welcome , " as customer\n");
    welcome[name_bytes+20] = '\0';
    // char* ne
    // write(1 , welcome , strlen(welcome));
    // intername 

    /// set

    fd_set master_set,working_set;

    

    FD_ZERO(&master_set);
    // FD_SET(server_fd, &master_set);






    char buffer[1024] = {0};
    struct sockaddr_in bc_address;

    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(udp_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(port); 
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");

    int connected_server_fd;
    connected_server_fd = connectServer(sever_port_val);

    FD_SET(udp_sock, &master_set);
    FD_SET(0, &master_set);
    // FD_SET(1, &master_set);
    int max_sd;
    max_sd = udp_sock;

    bind(udp_sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    sendto(udp_sock, welcome, strlen(welcome), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
    

    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        // if(kbhit()){
        //     char input_k[BUFFER_SIZE];
        //     int input_bytes_readed = read(0,input_k,BUFFER_SIZE);
        //     input_k[input_bytes_readed-1] = '/0';
        //     write(1,input_k,BUFFER_SIZE)
        // }
       
        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                if(i == udp_sock){
                    memset(buffer, 0, 1024);
                    int bytes_readed = recv(udp_sock, buffer, 1024,0);
                    write(1 , buffer , bytes_readed);
                }

                else if(i == 0){
                    char input_r[BUFFER_SIZE] ;
                    int bytes_input_r = read(0,input_r,BUFFER_SIZE);

                    if(input_r[0] == 's'){
                        char fd_str[40];
                        snprintf(fd_str , sizeof(fd_str) , "%d" , connected_server_fd);
                        char send_fd[BUFFER_SIZE] = "";
                        strcat(send_fd , fd_str);
                        strcat(send_fd , "\n");
                        
                        sendto(connected_server_fd, send_fd, sizeof(send_fd), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    }
                    
                    else{
                        sendto(udp_sock, input_r, bytes_input_r, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    }
                    // write(1 , input_r , bytes_input_r);
                    
                }
                else if(i == 1){
                    // write(1 , input_r , BUFFER_SIZE)
                    ;
                }
                
                else { 
                    // client sending msg
                    // int bytes_received;
                    // bytes_received = recv(i , buffer, 1024, 0);
                    
                    // if (bytes_received == 0) { // EOF
                    //     printf("client fd = %d closed\n", i);
                    //     close(i);
                    //     FD_CLR(i, &master_set);
                    //     continue;
                    // }

                    // printf("client %d: %s\n", i, buffer);
                    // memset(buffer, 0, 1024);
                    ;
                }
            }
        }
    }
    close(port);
    close(sever_port_val);
    return 0;
}
