//
// Created by pranath on 7/9/19.
//

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

bool server_listener = true;

void *reader(void *arg) {

    int connected_socket = *((int *)arg);

    while(server_listener) {

        //Receive Data
        char server_response[256];
        recv(connected_socket, &server_response, sizeof(server_response), 0);
        printf("%s\n", server_response);
        memset(server_response, 0, 256);
        usleep(100000);
    }
}

int main() {

    char message[2000];

    // Create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (network_socket < 0) {
        perror("Error in creating the network socket");
    }

    printf("Network Socket was Created Successfully!\n");

    // Specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect (network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    if (connection_status == -1) {
        perror("There was a connection error");
        return 0;
    }

    printf("Connection created successfully!\n");

    pthread_t reader_thread_id;
    pthread_create(&reader_thread_id, NULL, reader, &network_socket);

    while(server_listener) {
        scanf("%s", message);
        send(network_socket, message, strlen(message), 0);
        usleep(100000);
    }


    close(network_socket);

    return 0;

}