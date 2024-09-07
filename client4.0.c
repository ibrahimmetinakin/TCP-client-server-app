#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8081

// **** creating the socket ****
int create_socket()
{
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, 0))< 0 )
    {
        perror("failed to create socket, terminating program.");
    }
    return sock;
}

// **** setting the server address ****
int setup_server_address(struct sockaddr_in *serv_addr){
    serv_addr->sin_family = AF_INET; //set address family to IPv4
    serv_addr->sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr->sin_addr) <=0){
        perror("invalid address");
        return 0;  //error state
    }
    return 1;  //success state
}

// **** connecting to the server ****
int connect_to_server(int sock, struct sockaddr_in *serv_addr){
    if(connect(sock, (struct sockaddr *)serv_addr, sizeof(*serv_addr))<0){
        perror("connection failed\n");
        return 0; //error state
    }
    printf("connection successful \n");

    return 1;
}

//**** sending messages and receiving responses from the server ****
void send_and_receive_message(int sock, const char *message){
    char buffer[1024]={0};
    send(sock, message, strlen(message), 0);
    printf("\nMessage sent to server: %s\n",message);
   
    read(sock,buffer,1024);
    printf("Server response: %s\n",buffer);
}

//***** main function *****
int main(){
    int sock;
    struct sockaddr_in serv_addr;
    const char *message = "What time is it?";
    printf("*********************\n");
    
    sock=create_socket();
    if(sock<0){
        return 0;    //if the socket creation failed, exit
    }

    if(!setup_server_address(&serv_addr)){
        close(sock); //if the server address fails,close the socket
        return 0;    //exit
    }

    if(!connect_to_server(sock, &serv_addr)){
        close(sock); //if connection fails , close the socket 
        return 0;    //exit
    }

    send_and_receive_message(sock, message);
    close(sock);
    return 0;  // terminate the program successfully
}