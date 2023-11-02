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
    strcat(welcome , " as supplier\n");
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
    int connected_restaurant;
    int new_req = 0;
    while (1) {
        
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);
       
        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == server_fd) {  // new clinet
                    int connected_restaurant = acceptClient(server_fd);
                    FD_SET(connected_restaurant, &master_set);
                    if (connected_restaurant > max_sd)
                        max_sd = new_socket;
                    new_req=1;
                    char* new_restaurant = "New restaurant connected\n";
                    write(1,new_restaurant, strlen(new_restaurant));
                    
                }

                else if(i == udp_sock){
                    memset(buffer, 0, 1024);
                    int bytes_readed = recv(udp_sock, buffer, 1024,0);
                    buffer[bytes_readed] = '\0';
                    if(strcmp(buffer , "username and port :\n") == 0){
                        buffer[bytes_readed] = '\0';


                        char fd_str[40];
                        snprintf(fd_str , sizeof(fd_str) , "%d" , port_tcp);
                        char send_fd[BUFFER_SIZE] = "r";
                        strcat(send_fd , name);
                        strcat(send_fd , " ");
                        strcat(send_fd , fd_str);
                        strcat(send_fd , "\n");
                        
                        sendto(udp_sock, send_fd, sizeof(send_fd), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    
                    }else if(buffer[0] == 'r'){
                        ;
                    }
                    else if(buffer[0] == 'c'){
                        ;
                    }
                    else if(buffer[0] == 'w'){
                        write(1  , buffer , bytes_readed);
                    }
                    else{
                        write(1  , buffer , bytes_readed);
                    }
                }

                else if(i == 0){
                    memset(buffer, 0, 1024);
                    int bytes_readed = read(0, buffer, BUFFER_SIZE);
                    buffer[bytes_readed] = '\0';
                    if(strcmp(buffer , "answer request\n") == 0 && new_req){
                        
                        char get_res_port[BUFFER_SIZE];
                        
                        char* get_request = "\033[0;34mport \033[0;37mof request: " ;
                        write(1 , get_request , strlen(get_request));
                        int read_p_bytes = read(0 , get_res_port , BUFFER_SIZE);
                        get_res_port[read_p_bytes - 1] = '\0';

                        int res_p_new;
                        int res_fd_new;
                        res_p_new = atoi(get_res_port);
                        res_fd_new = connectServer(res_p_new);


                        char* req_ans_show = "your answer is(\033[0;32myes\033[0;37m/\033[0;31mno\033[0;37m) : ";
                        write(1 , req_ans_show , strlen(req_ans_show));

                        char restaurant_ans[BUFFER_SIZE];
                        int read_ans = read(0 , restaurant_ans , BUFFER_SIZE);
                        restaurant_ans[read_ans] = '\0';


                        sendto(res_fd_new, restaurant_ans, read_ans, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        new_req=0;
                        ;
                    }
                    
                }
                else if(i == 1){
                    
                }
                
                else { 
                    memset(buffer, 0, 1024);
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    buffer[bytes_received] = '\0';
                    write(1 , buffer , bytes_received);
                    
                    ;
                }
            }
        }
    }
    close(port);
    close(sever_port_val);
    return 0;
}
