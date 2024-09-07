#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8081

// **** creating the socket ****
int create_server_socket(){
    int server_fd;  // file descriptor
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0))== 0){
        perror("socket creation failed");
        return -1;  //error state
    }
    return server_fd;
}

// **** setting address and port ****
int setup_address(struct sockaddr_in *address){
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    return 1;  //success state
}

// **** connecting socket to port ****
int bind_socket(int server_fd , struct sockaddr_in *address){
    if(bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0 ){
       perror("bind failed");
       return 0; //error state
    }
    return 1;  //success state
}

// **** server listening on port ****
int start_listening(int server_fd){
    if(listen(server_fd, 3) < 0 ){
        perror("listen failed");
        return 0;  //error state
    }
    printf("server is listening on port %d\n",PORT);
    return 1; //success state
}

// **** accept connection ****
int accept_connection(int server_fd, struct sockaddr_in *address, int *addrlen) {
    int new_socket;
    if ((new_socket = accept(server_fd, (struct sockaddr *)address, (socklen_t*)addrlen)) < 0) {
        perror("Accept failed");
        return -1; // error state
    }
    return new_socket;
}

// **** reading data from client ****
void read_from_client(int new_socket, char *buffer, size_t buffer_size) {
    read(new_socket, buffer, buffer_size);
}

// **** creating an answer ****
void create_response(const char *request, char *response, size_t response_size) {
    if (strcmp(request, "What time is it?") == 0) {
        time_t t;
        struct tm *tm_info;
        char time_str[50];

        time(&t);
        tm_info = localtime(&t);
        strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);

        snprintf(response, response_size, "The current time is: %s", time_str);
    } else {
        snprintf(response, response_size, "Unknown request.");
    }
}

// **** send an answer ****
void send_to_client(int new_socket, const char *response) {
    send(new_socket, response, strlen(response), 0);
    printf("Response sent to client.\n");
}

//*********** main function **********
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char response[1024] = {0};

    server_fd = create_server_socket();
    if (server_fd < 0) {
        return 1; // error state
    }

     if (!setup_address(&address)) {
        return 1; // error state
    }

    if (!bind_socket(server_fd, &address)) {
        return 1; // error state
    }

    if (!start_listening(server_fd)) {
        return 1; // error state
    }

    new_socket = accept_connection(server_fd, &address, &addrlen);
    if (new_socket < 0) {
        return 1; // error state
    }

    read_from_client(new_socket, buffer, sizeof(buffer));
    printf("Client: %s\n", buffer);
    printf("*********************\n");

    create_response(buffer, response, sizeof(response));

    send_to_client(new_socket, response);

    close(new_socket);
    close(server_fd);

    return 0;
}
