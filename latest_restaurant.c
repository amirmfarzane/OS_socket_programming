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
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define sever_port_val 1024 + rand()%(49151-1024+1)



int setupServer(int port) {
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    listen(server_fd, 4);

    return server_fd;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return client_fd;
}

char* get_usename(){
    char* enter_name = "Please enter your username:";
    write(1 , enter_name , strlen(enter_name));
    char name[BUFFER_SIZE] =  {0};
    
    int name_bytes = read(0 , name , BUFFER_SIZE);
    name[name_bytes - 1] = '\0';

    static char welcome[BUFFER_SIZE];
    strcpy(welcome , "welcome ");
    strcat(welcome , name );
    strcat(welcome , " as resturant\n");
    welcome[name_bytes+21] = '\0';
    return welcome;
}


int main(int argc, char const *argv[]) {
    int udp_port = atoi(argv[1]);
    int udp_sock, broadcast = 1, opt = 1;


    char* welcome;
    welcome = get_usename();


    //sever 
    int server_fd,max_sd;
    server_fd = setupServer(sever_port_val);


    //set ::::
    
    fd_set master_set,working_set;

    

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    
    write(1, "Server is running\n", 18);

    
    char buffer[1024] = {0};
    char new_buffer[1024] = {0};
    struct sockaddr_in bc_address;




    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(udp_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(udp_port); 
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");

    bind(udp_sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    FD_SET(udp_sock, &master_set);
    FD_SET(0, &master_set);
    // FD_SET(1, &master_set);
    max_sd = udp_sock;
    sendto(udp_sock, welcome, strlen(welcome), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
  
    while(1){

        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);
       
        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == server_fd) {  // new clinet
                    int new_socket = acceptClient(server_fd);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                    printf("New client connected. fd = %d\n", new_socket);

                    
                    ;
                }
                else if(i == udp_sock){
                    memset(buffer, 0, 1024);
                    int bytes_readed = recv(udp_sock, buffer, 1024,0);
                    buffer[bytes_readed] = '\0';
                    if(strcmp(buffer , "srs\n") == 0){
                        buffer[bytes_readed-1] = '\0';
                        char fd_str[40];
                        snprintf(fd_str , sizeof(fd_str) , "%d" , server_fd);
                        char send_fd[BUFFER_SIZE] = "s ";
                        strcat(send_fd , fd_str);
                        strcat(send_fd , "\n");
                        
                        sendto(udp_sock, send_fd, sizeof(send_fd), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    
                    }
                    write(1 , buffer , bytes_readed);
                }

                else if(i == 0){
                    char input_r[BUFFER_SIZE] ;
                    int bytes_input_r = read(0,input_r,BUFFER_SIZE);

                    if(input_r[0] == 'c'){
                        // sendto(server_fd, input_r, bytes_input_r, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        ;
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
                    memset(buffer, 0, 1024);
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    
                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    write(1 , buffer , bytes_received);
                    
                    ;
                }
            }
        }
        }
        close(udp_port);
        close(sever_port_val);

    

    return 0;
}
