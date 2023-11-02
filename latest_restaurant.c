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
#include <fcntl.h> 
#include <stdio.h> 
#include <unistd.h> 
#include "cJSON/cJSON.h"

#define sever_port_val 7093
#define BUFFER_SIZE 1024
#define recepies_num 15

int input_locked = 0;

void read_recipes_file(){
    int fd = open("recipes.json", O_RDONLY); 
	if (fd == -1) { 
        char* error = "Error: Unable to open the file.\n";
		write(1,error,strlen(error)); 
	} 

	// read the file contents into a string 
	char buffer[10024]; 
	int len = read(fd, buffer, 10024); 
	// fclose(fp); 

	// parse the JSON data 
	cJSON *root = cJSON_Parse(buffer); 

	cJSON *item;
	cJSON *dish;
    char* dash = "------------------\n";
    write(1,dash,strlen(dash));
    int num=1;
	cJSON_ArrayForEach(item,root){

		const char* dish_name = item->string;

		char fd_str[40];
        snprintf(fd_str , sizeof(fd_str) , "%d" , num);
        char send_fd[BUFFER_SIZE] = "";
        strcat(send_fd , fd_str);
        strcat(send_fd , ". \033[0;31m");
        strcat(send_fd , dish_name);
        strcat(send_fd ,"\033[0;37m:\n");
        write(1 , send_fd , strlen(send_fd));
        memset(fd_str,0,40);
        memset(send_fd,0,1024);
		cJSON_ArrayForEach(dish,item){
			const char* ingre = dish->string;
			int val = dish->valueint;
			char fd_str[40];
            snprintf(fd_str , sizeof(fd_str) , "%d" , val);
            char send_fd[BUFFER_SIZE] = "   \033[0;35m";
            strcat(send_fd , ingre);
            strcat(send_fd , "  \033[0;37m: \033[0;38m");
            strcat(send_fd , fd_str);
            strcat(send_fd ,"\033[0;37m\n");
            write(1 , send_fd , strlen(send_fd));
            memset(fd_str,0,40);
            memset(send_fd,0,1024);
	}
    num = num+1;
    write(1,dash,strlen(dash));
	}
	
	
	cJSON_Delete(root); 
	close(fd);
}
// #define sever_port_val 1024 + rand()%(49recepies_num1-1024+1)

struct server_fd_port{
    int fd;
    int port;
};


void alarm_handler(int sig){
    char *buffer;
    buffer = "\033[0;37mtime finished\n";

    write(1 , buffer , strlen(buffer));
    ;
}

struct server_fd_port setupServer() {
    srand(time(NULL));
    struct sockaddr_in address;
    struct server_fd_port my_server_info;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    int port ;
    port = 1024 + rand()%1000;
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

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return client_fd;
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
    strcat(welcome , " as resturant\n");
    welcome[name_bytes+21] = '\0';

    struct welome_name result;
    result.welcome  =welcome;
    result.name = name;
    return result;
}

struct ingred{
    char* name;
    int number;
};

struct ingred ingreds[16];

void impact_accept_ingred(int val,char* name){
    for(int i=0 ; i<recepies_num ; i++){
        if(strcmp(ingreds[i].name , name)==0){
            ingreds[i].number = ingreds[i].number+val;
            printf("hi\n");
        }
    }
}



char client_port_name[BUFFER_SIZE];
int client_port_val;


int main(int argc, char const *argv[]) {
    int udp_port = atoi(argv[1]);
    int udp_sock, broadcast = 1, opt = 1;
    
    
    ingreds[0].name = "beef";
    ingreds[0].number = 0;
    ingreds[1].name = "Onions";
    ingreds[1].number = 0;
    ingreds[2].name = "Salt";
    ingreds[2].number = 0;
    ingreds[3].name = "Saffron";
    ingreds[3].number = 0;
    ingreds[4].name = "Stew-beef";
    ingreds[4].number = 0;
    ingreds[5].name = "Parsely";
    ingreds[5].number = 0;
    ingreds[6].name = "Turmeric";
    ingreds[6].number = 5;
    ingreds[7].name = "Basmati-rice";
    ingreds[7].number = 0;
    ingreds[8].name = "Sugar";
    ingreds[8].number = 0;
    ingreds[9].name = "Eggplant";
    ingreds[9].number = 2;
    ingreds[10].name = "Garlic";
    ingreds[10].number = 0;
    ingreds[11].name = "Walnuts";
    ingreds[11].number = 0;
    ingreds[12].name = "Eggs";
    ingreds[12].number = 0;
    ingreds[13].name = "Chickpeas";
    ingreds[13].number = 0;
    ingreds[14].name = "Lentils";
    ingreds[14].number = 0;

    struct welome_name welocme_name_struct;
    welocme_name_struct = get_usename();

    char* welcome;
    welcome = welocme_name_struct.welcome;
    char* name ;
    name = welocme_name_struct.name;


    //sever 
    int port_tcp,server_fd,max_sd;
    struct server_fd_port fd_port;
    fd_port = setupServer();
    server_fd = fd_port.fd;
    port_tcp = fd_port.port;


    //set ::::
    
    fd_set master_set,working_set;

    

    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    

    
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
  
    char get_port_req[BUFFER_SIZE];
    char get_name_req[BUFFER_SIZE];
    char get_number_req[BUFFER_SIZE];
    

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
                }
                else if(i == udp_sock){
                    memset(buffer, 0, 1024);
                    int bytes_readed = recv(udp_sock, buffer, 1024,0);
                    buffer[bytes_readed] = '\0';
                    if(strcmp(buffer , "username and port : \n") == 0){
                        buffer[bytes_readed] = '\0';


                        char fd_str[40];
                        snprintf(fd_str , sizeof(fd_str) , "%d" , port_tcp);
                        char send_fd[BUFFER_SIZE] = "c";
                        strcat(send_fd , name);
                        strcat(send_fd , " ");
                        strcat(send_fd , fd_str);
                        strcat(send_fd , "\n");
                        
                        sendto(udp_sock, send_fd, sizeof(send_fd), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    
                    }else if((strcmp(buffer , "request list\n") == 0) || buffer[0] == 'c'){
                        ;
                    }
                    else if(buffer[0] == 'r'){
                        buffer[0]=' ';
                        write(1 , buffer , bytes_readed);
                    }
                    else if(buffer[0] == 'w'){
                        write(1  , buffer , bytes_readed);
                    }else if(strcmp(buffer , "username and port :\n") == 0){
                        write(1 , "username and port :\n",21);
                    
                    }else{
                        // write(1 , buffer , bytes_readed);
                    }
                }

                else if(i == 0){
                    char input_r[BUFFER_SIZE] ;
                    int bytes_input_r = read(0,input_r,BUFFER_SIZE);
                    input_r[bytes_input_r] = '\0';
                    if(strcmp(input_r , "answer request\n") == 0){
                        memset(client_port_name , 0 , BUFFER_SIZE);
                        
                        char* get_request = "\033[0;34mport \033[0;37mof request: " ;
                        write(1 , get_request , strlen(get_request));
                        int read_name = read(0 , client_port_name , BUFFER_SIZE);
                        client_port_name[read_name - 1] = '\0';

                        
                        
                        client_port_val = atoi(client_port_name);
                        client_port_val = connectServer(client_port_val);

                        char* req_ans_show = "your answer is(\033[0;32myes\033[0;37m/\033[0;31mno\033[0;37m) : ";
                        write(1 , req_ans_show , strlen(req_ans_show));

                        char restaurant_ans[BUFFER_SIZE];
                        int read_ans = read(0 , restaurant_ans , BUFFER_SIZE);
                        restaurant_ans[read_ans] = '\0';


                        sendto(client_port_val, restaurant_ans, read_ans, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        FD_SET(client_port_val, &master_set);
                        ;
                    }

                    else if(strcmp(input_r , "request list\n") == 0){
                        sendto(udp_sock, input_r, bytes_input_r, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        ;
                    }
                    
                    else if(strcmp(input_r , "show recipes\n") == 0){
                        read_recipes_file();
                    }

                    else if(strcmp(input_r , "show ingredients\n") == 0){
                        char ing_amount[20] = "ingre and amount : \n";
                        write(1 , ing_amount , strlen(ing_amount));
                        for(int j=0;j<recepies_num;j++){
                            if(0 < ingreds[j].number){
                                write(1 , ingreds[j].name , strlen(ingreds[j].name ));
                                write(1 , " " , 1);
                                write(1 , ingreds[j].number , sizeof(int));
                                write(1 , "\n" , 1);
                            }
                        }
                    }

                    else if(strcmp(input_r , "show suppliers\n") == 0){
                        char username[BUFFER_SIZE] = "username and port :\n";
                        
                        sendto(udp_sock, username, BUFFER_SIZE, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        // sendto()
                    }

                    else if(strcmp(input_r , "request ingredient\n") == 0){
                        

                        char my__port[40];
                        snprintf(my__port , sizeof(my__port) , "%d" , port_tcp);

                        char send__fd[BUFFER_SIZE] = "";
                        strcat(send__fd , "my port is: ");
                        strcat(send__fd , my__port);
                        strcat(send__fd , "\n");
                        
                        char* get_port_name = "\033[0;32mport \033[0;37mof supplier : " ;
                        write(1 , get_port_name , strlen(get_port_name));
                        int read_port_bytes = read(0 , get_port_req , BUFFER_SIZE);
                        get_port_req[read_port_bytes - 1] = '\0';

                        char* get_name_name = "name of \033[0;32mingredient \033[0;37m: " ;
                        write(1 , get_name_name , strlen(get_name_name));
                        int read_name_bytes = read(0 , get_name_req , BUFFER_SIZE);
                        get_name_req[read_name_bytes - 1] = '\0';

                        char* get_num_name = "number of \033[0;32mingredient \033[0;37m: " ;
                        write(1 , get_num_name , strlen(get_num_name));
                        int read_num_bytes = read(0 , get_number_req , BUFFER_SIZE);
                        get_number_req[read_num_bytes - 1] = '\0';


                        // write(1 , port_number ,read_port - 1 );
                        int connected_supp_port , connected_supp_fd;
                        connected_supp_port = atoi(get_port_req);
                        connected_supp_fd = connectServer(connected_supp_port);
                        
                        char* new_order = "\033[0;32mnew \033[0;37mrequest ingredient\n";
                        char* waiting = "waiting for respones\n";
                        write(1 , waiting , strlen(waiting));

                        sendto(connected_supp_fd, new_order, strlen(new_order), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        sendto(connected_supp_fd, send__fd, strlen(send__fd), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        FD_SET(connected_supp_fd, &master_set);
                        signal(SIGALRM, alarm_handler);
                        unsigned int second = 25;
                        alarm(second);
                        
                    }
                    else if(input_r[0] == 'r'){
                       input_r[0] == ' ' ;
                       write(1  , input_r , bytes_input_r);
                    }
                    else{
                        // sendto(udp_sock, input_r, bytes_input_r, 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        ;
                    }
                }
                else { 
                    memset(buffer, 0, 1024);
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    buffer[bytes_received] = '\0';
                    if(strcmp(buffer , "yes\n") == 0){
                        int num = atoi(get_number_req);
                        impact_accept_ingred(num , get_name_req);
                    }
                    write(1 , buffer , bytes_received);
                    
                    ;
                }
            }
        }
        }
        close(udp_port);
        close(port_tcp);

    

    return 0;
}
