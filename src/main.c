/*
 * main.c
 * 
 * Copyright 2020  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define PORT 8080
void setHttpHeader(char httpHeader[]);
void setHttpHeader_other(char httpHeader[], char *path);
void report(struct sockaddr_in *serverAddress);
char* parse(char line[]);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    char httpHeader[100000] = "HTTP/1.1 200 OK\r\n\n";
    char httpHeader1[3800000] = "HTTP/1.1 200 OK\r\n\n";
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In sockets");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    
    //report(&address);
    setHttpHeader(httpHeader);
    
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        
        //printf("%s\n",buffer );
        
        char *parse_string = parse(buffer);
        printf("path: %s\n", parse_string);
        if(strlen(parse_string) <= 1){
            write(new_socket , httpHeader , strlen(httpHeader));
            printf("\n Send index.html file \n");
        }
        else{
            setHttpHeader_other(httpHeader1, parse_string);
            write(new_socket , httpHeader1 , strlen(httpHeader1));
            printf("\n Send other file, size: %d \n", strlen(httpHeader1));
            printf("%s", httpHeader1);
        }
        //printf("This is the pathe: %s", parse_path);
        printf("------------------Server sent-------------------\n");
        close(new_socket);
        //free(parse_string);
    }
    return 0;
}

void setHttpHeader(char httpHeader[])
{
    
    // File object to return
    
    FILE *htmlData = fopen("inde.html", "r");
    //FILE *htmlData = fopen("./img/inde.html", "r");


    char line[100];
    char responseData[1000000];
    if(htmlData){
        while (fgets(line, 100, htmlData) != 0 ) {
            strcat(responseData, line);
        }
        // char httpHeader[8000] = "HTTP/1.1 200 OK\r\n\n";
        strcat(httpHeader, responseData);
        fclose(htmlData);
    }
    else
    {
        printf("\n Read index.html file problem");
    }
}

void setHttpHeader_other(char httpHeader[], char *path)
{
    
    // File object to return
    
    char path_head[500] = ".";
    strcat(path_head, path);
    printf("\n path head : %s", path_head);
    
    FILE *htmlData1 = fopen(path_head, "r");

    ////char httpHeader1[8000] = "HTTP/1.1 200 OK\r\n\n";
    char line[100];
    char responseData[3800000];
    if(htmlData1){
        while (fgets(line, 100, htmlData1) != 0 ) {
            strcat(responseData, "TTT");
        }
        strcat(httpHeader, responseData);
        fclose(htmlData1);
    }
    else
    {
        printf("\n Read other file problem");
    }
}

void report(struct sockaddr_in *serverAddress)
{
    char hostBuffer[INET6_ADDRSTRLEN];
    char serviceBuffer[NI_MAXSERV]; // defined in `<netdb.h>`
    socklen_t addr_len = sizeof(*serverAddress);
    int err = getnameinfo(
        (struct sockaddr *) serverAddress,
        addr_len,
        hostBuffer,
        sizeof(hostBuffer),
        serviceBuffer,
        sizeof(serviceBuffer),
        NI_NUMERICHOST
    );
    if (err != 0) {
        printf("It's not working!!\n");
    }
    printf("\n\n\tServer listening on http://%s:%s\n", hostBuffer, serviceBuffer);
}

char* parse(char line[])
{
    //char *path;
    char *message;
    char * token = strtok(line, " ");
    int current = 0;
    
    message = "No path";
    printf("token--------------------\n");
    while( token != NULL ) {
      if(current < 2){
          printf( " %s\n", token ); //printing each token
      }
      
      token = strtok(NULL, " ");
      if(current == 0){
          message = token;
          return message;
      }
      current = current + 1;
      
   }
   return message;
    
}

