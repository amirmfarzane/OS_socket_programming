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
#include <termio.h>

#define sever_port_val 7093
#define BUFFER_SIZE 1024

int input_locked = 0;

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

int connectServer(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { // checking for errors
        printf("Error in connecting to server\n");
    }

    return fd;
}

struct welome_name{
    char* welcome;
    char* name;
};

struct welome_name get_usename(){
    char* enter_name = "Please enter your username:";
    write(1 , enter_name , strlen(enter_name));
    static char name[BUFFER_SIZE] =  {0};
    
    int name_bytes = read(0 , name , BUFFER_SIZE);
    name[name_bytes - 1] = '\0';

    static char welcome[BUFFER_SIZE];
    strcpy(welcome , "welcome ");
    strcat(welcome , name );
    strcat(welcome , " as client\n");
    welcome[name_bytes+21] = '\0';

    struct welome_name result;
    result.welcome  =welcome;
    result.name = name;
    return result;
}

char* client_port_show(int server_fd){
    char fd_str[40];
    snprintf(fd_str , sizeof(fd_str) , "%d" , server_fd);
    static char send_fd[BUFFER_SIZE] = "";
    strcat(send_fd , "my port is : ");
    strcat(send_fd , " ");
    strcat(send_fd , fd_str);
    strcat(send_fd , "\n");
    return send_fd;
 }

void alarm_handler(int sig){
    if(input_locked)
        input_locked =0;
}


int main(int argc, char const *argv[]) {
    int port = atoi(argv[1]);
    int udp_sock, broadcast = 1, opt = 1;
    int client_port = 2048 + rand()%1000;
    //inter name:
    struct welome_name welocme_name_struct;
    welocme_name_struct = get_usename();

    char* welcome;
    welcome = welocme_name_struct.welcome;
    char* name ;
    name = welocme_name_struct.name;

    /// set

    fd_set master_set,working_set;

    int server_fd,max_sd;
    server_fd = setupServer(sever_port_val);
    

    FD_ZERO(&master_set);

    char buffer[1024] = {0};
    struct sockaddr_in bc_address;

    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(udp_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(port); 
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");

    // int connected_server_fd;
    // connected_server_fd = connectServer(9999);

    FD_SET(server_fd, &master_set);
    FD_SET(udp_sock, &master_set);
    FD_SET(0, &master_set);
    // FD_SET(1, &master_set);
    
    max_sd = udp_sock;

    bind(udp_sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    sendto(udp_sock, welcome, strlen(welcome), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
    char food_name[BUFFER_SIZE];
    static char port_number[BUFFER_SIZE];
    int connected_server_port , connected_server_fd;

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
                    // write(1, "\033[0;35m" , 8);
                    if(strcmp(buffer , "request list\n") == 0){
                        char fd_str[40];
                        snprintf(fd_str , sizeof(fd_str) , "%d" , client_port);
                        char send_fd[BUFFER_SIZE] = "";
                        strcat(send_fd , name);
                        strcat(send_fd , " ");
                        strcat(send_fd , fd_str);
                        strcat(send_fd , " ");
                        strcat(send_fd , food_name);
                        strcat(send_fd , "\n");

                        
                        sendto(connected_server_fd, send_fd, strlen(send_fd), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    }else{
                        write(1  , buffer , bytes_readed);
                    }
                }
                else if(i == server_fd){

                }

                else if(i == 0){
                    char input_r[BUFFER_SIZE] ;
                    char welcome_r[BUFFER_SIZE];
                    int bytes_input_r = read(0,input_r,BUFFER_SIZE);
                    input_r[bytes_input_r ] = '\0';
                    welcome_r[7] = '\0';
                    // if(input_r == "show restaurants"){
                    //     char fd_str[40];
                    //     snprintf(fd_str , sizeof(fd_str) , "%d" , connected_server_fd);
                    //     char send_fd[BUFFER_SIZE] = "s ";
                    //     strcat(send_fd , fd_str);
                    //     strcat(send_fd , "\n");
                        
                    //     sendto(connected_server_fd, send_fd, sizeof(send_fd), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    // }

                    if(strcmp(input_r , "show restaurants\n") == 0){
                        char username[BUFFER_SIZE] = "username and port : \n";
                        
                        sendto(udp_sock, username, BUFFER_SIZE, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        // sendto()
                    }

                    else if(strcmp(input_r , "order food\n") == 0){
                        memset(food_name , 0 , BUFFER_SIZE);
                        memset(port_number , 0 , BUFFER_SIZE);
                        strcat(food_name ,"\033[0;34m");
                        char* get_food_name = "write food name : " ;
                        write(1 , get_food_name , strlen(get_food_name));
                        int read_name = read(0 , food_name , BUFFER_SIZE);
                        food_name[read_name - 1] = '\0';

                        char* get_port_number = "write restaurant port : " ;
                        write(1 , get_port_number , strlen(get_port_number));
                        int read_port = read(0 , port_number , BUFFER_SIZE);
                        port_number[read_port - 1] = '\0';
                        // write(1 , port_number ,read_port - 1 );
                        
                        connected_server_port = atoi(port_number);
                        connected_server_fd = connectServer(connected_server_port);
                        char* new_order = "\033[0;32mnew \033[0;37morder\n";
                        sendto(connected_server_fd, new_order, strlen(new_order), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        FD_SET(connected_server_fd, &master_set);
                    }
                    else if(strcmp(welcome_r , "welcome") == 0){
                        sendto(udp_sock, input_r, bytes_input_r, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    }
                    
                    else{
                        ;
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
