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
#include <inttypes.h>

bool server_listener = true;

void *reader(void *arg) {

    int connected_socket = *((int *)arg);
    int receive_message_size = 0;
    char msg_size_buffer[5];

    while( (receive_message_size = recv(connected_socket , msg_size_buffer , sizeof(msg_size_buffer) , 0)) > 0 ) {

        if ( receive_message_size <= 0 ) {
            perror("Error Receiving the Message\n");
        }

        int msg_size[5]={msg_size_buffer[0] - '0', msg_size_buffer[1] - '0', msg_size_buffer[2] - '0', msg_size_buffer[3] - '0', msg_size_buffer[4] - '0'};
        char msg_size_str[5];
        int i=0;
        int index = 0;
        for (i=0; i<5; i++) {
            index += snprintf(&msg_size_str[index], 6-index, "%d", msg_size[i]);
        }
        memset(msg_size_buffer, 0, 5);
        char* strtoumax_endptr;
        int msg_size_int = strtoumax(msg_size_str, &strtoumax_endptr, 10);

        char *buffer = NULL;
        buffer = malloc( (msg_size_int + 2)*sizeof(char) + 1);

        int receive_message_content_size = recv(connected_socket, buffer, (msg_size_int + 2)*sizeof(char), 0);

        if (receive_message_content_size <= 0) {
            perror("Error with message");
        }

        printf("%s\n", buffer);

        free(buffer);
    }
}


char *input_string(FILE* fp, size_t size){
    char *str = NULL;
    int ch;
    size_t len = 0;
    str = malloc(sizeof(char)*size);

    if(!str) {
        return str;
    }

    while(EOF != (ch = fgetc(fp)) && ch != '\n'){
        str[len++] = ch;
        if(len == size){
            str = realloc(str, sizeof(char)*(size += 16));
            if(!str)return str;
        }
    }
    str[len++] = '\0';

    return realloc(str, sizeof(char)*len);
}


int main() {

    char message[2000];

    // Create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    printf("net sock: %d\n", network_socket);
    if (network_socket == -1) {
        perror("Error in creating the network socket");
        return 0;
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
//        scanf("%s", message);

        char *m;
        char msg_size_str[6];
        char *full_msg;
        m = input_string(stdin, 10);
        sprintf(msg_size_str, "%05d", (int) strlen(m) + 25);
        full_msg = malloc(sizeof(char)*(strlen(m) + 25) + 1);
        strcpy(full_msg, msg_size_str);
        strcat(full_msg, m);
        printf("Full Message: %s\n", full_msg);
        send(network_socket, full_msg, strlen(full_msg), 0);
        free(m);
        free(full_msg);
        usleep(100000);
    }

    close(network_socket);

    return 0;

}