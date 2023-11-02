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
#include <time.h>

#define sever_port_val 7093
#define BUFFER_SIZE 1024

int input_locked = 0;

struct server_fd_port{
    int fd;
    int port;
};

struct server_fd_port setupServer() {
    srand(time(NULL));
    struct sockaddr_in address;
    struct server_fd_port my_server_info;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    int port ;
    port = 2024 + rand()%1000;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    my_server_info.fd = server_fd;
    my_server_info.port = port;
    
    listen(server_fd, 4);

    write(1, "Server is running\n", 18);
    return my_server_info;
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
        write(1,"Error connection\n",17);
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
    char *buffer;
    buffer = "\033[0;37mtime finished\n";

    write(1 , buffer , strlen(buffer));
    ;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return client_fd;
}



int main(int argc, char const *argv[]) {
    int new_socket;
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
    int port_tcp,server_fd,max_sd;

    struct server_fd_port fd_port;
    fd_port = setupServer();
    server_fd = fd_port.fd;
    port_tcp = fd_port.port;
    

    FD_ZERO(&master_set);

    char buffer[BUFFER_SIZE] = {0};
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
                if(i == server_fd){
                    new_socket = acceptClient(server_fd);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;

                    // printf("New restaurant connected. fd = %d\n", new_socket);
                    // memset(buffer, 0, 1024);
                    // int bytes_readed = recv(0, buffer, 1024,0);
                    // buffer[bytes_readed] = '\0';
                    // sendto(new_socket, buffer, sizeof(bytes_readed), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    
                }

                else if(i == udp_sock){
                    memset(buffer, 0, 1024);
                    int bytes_readed = recv(udp_sock, buffer, 1024,0);
                    // write(1, "\033[0;35m" , 8);
                    if(strcmp(buffer , "request list\n") == 0){
                        char fd_str[40];
                        snprintf(fd_str , sizeof(fd_str) , "%d" , port_tcp);
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
                        char* waiting = "waiting for respones\n";
                        write(1 , waiting , strlen(waiting));
                        sendto(connected_server_fd, new_order, strlen(new_order), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));

                        // signal(SIGALRM, alarm_handler);
                        // unsigned int second = 55;
                        // alarm(second);
                        FD_SET(connected_server_fd, &master_set);
                    }
                    else if(strcmp(welcome_r , "welcome") == 0){
                        sendto(udp_sock, input_r, bytes_input_r, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    }
                    else if(strcmp(welcome_r , "yes") == 0){
                        ;
                    }
                    else if(strcmp(welcome_r , "no") == 0){
                        ;
                    }
                    
                    else{
                        char* unvalid = "unvalid input\n";
                        write(1 , unvalid , strlen(unvalid));;
                    }
                    // write(1 , input_r , bytes_input_r);
                    
                }
                else if(i == 1){
                    // memset(buffer, 0, 1024);
                    // int bytes_received;
                    // bytes_received = recv(connected_server_fd , buffer, 1024, 0);
                    // buffer[bytes_received-1] = '\0';
                    // if (bytes_received == 0) { // EOF
                    //     printf("client fd = %d closed\n", i);
                    //     close(i);
                    //     FD_CLR(i, &master_set);
                    //     continue;
                    // }
                    

                    // write(1 , buffer , bytes_received);
                    // ;
                }
                
                else { 
                    memset(buffer, 0, 1024);
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    buffer[bytes_received] = '\0';
                    if(strcmp(buffer , "yes\n") == 0){
                        close(i);
                        char* req_ac = "req accepted\n";
                        write(1 , req_ac , strlen(req_ac));
                    }else if(strcmp(buffer , "no\n") == 0){
                        close(i);
                        char* req_ac = "req rejected\n";
                        write(1 , req_ac , strlen(req_ac));
                    }else{
                        write(1 , buffer , bytes_received);
                    }
                    
                    ;
                }
            }
        }
    }
    close(port);
    close(sever_port_val);
    return 0;
}
