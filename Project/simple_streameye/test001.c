/*
 * test001.c
 * 
 * Copyright 2022  <pi@raspberrypi>
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

//https://github.com/ccrisan/streameye

#include <stdio.h> 
#include <unistd.h> 

#include <string.h>   //strlen
#include <stdlib.h>   //malloc
#include <fcntl.h>  //read, fcntl

#include <errno.h> // header file defines the integer variable errno
#include <pthread.h>
#include <sys/time.h>  //gettimeofday
#include <netinet/in.h>  //INET_ADDRSTRLEN
#include <arpa/inet.h>   //inet_ntop
#include <pthread.h>
#include <sys/stat.h>  //stat

#include <signal.h> //singal
//undefined reference to 'pthread_create' with C program in GCC Linux
//https://www.includehelp.com/c-programming-questions/error-undefined-reference-to-pthread-create-in-linux.aspx

#define PORT 8085
#define JPEG_BUF_LEN            1024 * 1024 * 3 /* 10MB */
#define INPUT_BUF_LEN           1024 * 1024 /* 1MB */
#define JPEG_START              "\xFF\xD8"
#define JPEG_END                "\xFF\xD9"
#define BOUNDARY_SEPARATOR      "--FrameBoundary"
#define SOFTWARE_VERSION		"1.0.0"
#define REQ_BUF_LEN             4096

//#define INFO(fmt, ...)          fprintf(stderr, "%s: INFO : " fmt "\n", str_timestamp(), ##__VA_ARGS__)
//#define ERROR(fmt, ...)         fprintf(stderr, "%s: ERROR: " fmt "\n", str_timestamp(), ##__VA_ARGS__)
//#define ERRNO(msg)              ERROR("%s: %s", msg, strerror(errno))
//#define ERROR_CLIENT(client, fmt, ...)  ERROR("%s:%d: " fmt, client->addr, client->port, ##__VA_ARGS__)
//#define ERRNO_CLIENT(client, msg)       ERROR_CLIENT(client, "%s: %s", msg, strerror(errno))



typedef struct {
    int             stream_fd;
    char            addr[INET_ADDRSTRLEN];
    int             port;
    char            method[10];
    char            http_ver[10];
    char            uri[1024];
    char *          auth_basic_hash;
    pthread_t       thread;

    int             jpeg_ready;
    char *          jpeg_tmp_buf;
    int             jpeg_tmp_buf_size;
    int             jpeg_tmp_buf_max_size;

    double          frame_int;
    double          last_frame_time;
} client_t;

client_t *wait_for_client(int socket_fd);
void cleanup_client(client_t *client) ;
void  handle_client(client_t *client);
double get_now();
static int read_request(client_t *client);
int write_to_client(client_t *client, char *buf, int size);
int write_multipart_header(client_t *client, int jpeg_size);
int write_response_ok_header(client_t *client);
//int init_server();
void bye_handler(int signal);
//char *str_timestamp();

pthread_mutex_t jpeg_mutex;
pthread_cond_t jpeg_cond;
pthread_mutex_t clients_mutex;
static char *input_separator = NULL;
static int num_clients = 0;
static client_t **clients = NULL;
static int max_clients = 10;
const char *MULTIPART_HEADER =
        "\r\n" BOUNDARY_SEPARATOR "\r\n"
        "Content-Type: image/jpeg\r\n"
        "Content-Length: ";
const char *RESPONSE_OK_HEADER_TEMPLATE =
        "HTTP/1.1 200 OK\r\n"
        "Server: streamEye/%s\r\n"
        "Connection: close\r\n"
        "Max-Age: 0\r\n"
        "Expires: 0\r\n"
        "Cache-Control: no-cache, private\r\n"
        "Pragma: no-cache\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=" BOUNDARY_SEPARATOR "\r\n";

//extern int running;
//extern int jpeg_size;
//extern char jpeg_buf[];

int running = 1;
int jpeg_size = 0;
char jpeg_buf[JPEG_BUF_LEN];

int main(int argc, char *argv[]) 
{   
	
	char input_buf[INPUT_BUF_LEN];
	
	int size, rem_len = 0;
	
	int auto_separator = 0;
	char *sep = NULL;
	double frame_int = 0;
	double now, min_client_frame_int;;
	int input_separator_len;
	double last_frame_time = get_now();
	double frame_int_adj;
	
	int server_fd, new_socket, pid; 
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Creating socket file descriptor
    //server_fd = init_server();
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In sockets");
        exit(EXIT_FAILURE);
    }
   
    int tr = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(tr)) < 0) {
        printf("setsockopt() failed");
        return -1;
    }
    
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
        perror("In bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0){
        perror("In listen");
        close(server_fd);
        return -1;
        //exit(EXIT_FAILURE);
    }
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0) {
        printf("fcntl() failed");
        close(server_fd);
        return -1;
    }
    
    /* signals */
    /*	
    struct sigaction act;
    act.sa_handler = bye_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGINT, &act, NULL) < 0) {
        //ERRNO("sigaction() failed");
        return -1;
    }
    if (sigaction(SIGTERM, &act, NULL) < 0) {
        //ERRNO("sigaction() failed");
        return -1;
    }
    */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        //ERRNO("signal() failed");
        return -1;
    }
	
	if (pthread_cond_init(&jpeg_cond, NULL)) {
        printf("pthread_cond_init() failed \n");
        return -1;
    }
    if (pthread_mutex_init(&jpeg_mutex, NULL)) {
        printf("pthread_mutex_init() failed \n");
        return -1;
    }
    if (pthread_mutex_init(&clients_mutex, NULL)) {
        printf("pthread_mutex_init() failed \n");
        return -1;
    }
    
    if (!input_separator) {
        auto_separator = 1;
        input_separator_len = 4; /* strlen(JPEG_START) + strlen(JPEG_END) */;
        input_separator = malloc(input_separator_len + 1);
        snprintf(input_separator, input_separator_len + 1, "%s%s", JPEG_END, JPEG_START);
    }
    else{
        input_separator_len = strlen(input_separator);
    }
	
	while(running){
		size = read(STDIN_FILENO, input_buf, INPUT_BUF_LEN);
		if (size < 0) {
            if (errno == EINTR) {
                break;
            }
            //ERRNO("input: read() failed");
            printf("input: read() failed \n");
            running = 0;
            break;
            //return -1;
        }
        else if (size == 0) {
            //DEBUG("input: end of stream");
            running = 0;
            break;
        }

        if (size > JPEG_BUF_LEN - 1 - jpeg_size) {
            printf("input: jpeg size too large, discarding buffer");
            jpeg_size = 0;
            continue;
        }
        if (pthread_mutex_lock(&jpeg_mutex)) {
            printf("pthread_mutex_lock() failed");
            return -1;
        }
        
        /* clear the ready flag for all clients,
         * as we start building the next frame */
        for (int i = 0; i < num_clients; i++) {
            clients[i]->jpeg_ready = 0;
        }

        if (rem_len) {
            /* copy the remainder of data from the previous iteration back to the jpeg buffer */
            memmove(jpeg_buf, sep + (auto_separator ? 2 /* strlen(JPEG_END) */ : input_separator_len), rem_len);
            jpeg_size = rem_len;
        }

        memcpy(jpeg_buf + jpeg_size, input_buf, size);  //jpeg_buf pointer will move jpeg_size, starting from here put the input_buf with size into jpeg_buf
        //https://people.scs.carleton.ca/~sbtajali/1002/slides/11_PntrArith.pdf
        jpeg_size += size;

        /* look behind at most 2 * INPUT_BUF_LEN for a separator */
        int min_inputbufflen_jpeg = 2 * INPUT_BUF_LEN > jpeg_size ? jpeg_size : 2 * INPUT_BUF_LEN;
        sep = (char *) memmem(jpeg_buf + jpeg_size - min_inputbufflen_jpeg, min_inputbufflen_jpeg,
                input_separator, input_separator_len);

        if (sep) { /* found a separator, jpeg frame is ready */
            if (auto_separator) {
                rem_len = jpeg_size - (sep - jpeg_buf) - 2 /* strlen(JPEG_START) */;
                jpeg_size = sep - jpeg_buf + 2 /* strlen(JPEG_END) */;
            }
            else {
                rem_len = jpeg_size - (sep - jpeg_buf) - input_separator_len;
                jpeg_size = sep - jpeg_buf;
            }

            //printf("input: jpeg buffer ready with %d bytes \n", jpeg_size);
            
            for (int i = 0; i < num_clients; i++) {
                clients[i]->jpeg_ready = 1;               
            }
            if (pthread_cond_broadcast(&jpeg_cond)) {
                printf("pthread_cond_broadcast() failed");
                return -1;
            }
            now = get_now();
            frame_int = frame_int * 0.7 + (now - last_frame_time) * 0.3;
            last_frame_time = now;
        }
        else {
            rem_len = 0;
        }
        if (pthread_mutex_unlock(&jpeg_mutex)) {
            printf("pthread_mutex_unlock() failed");
            return -1;
        }
        if (sep) {
            //printf("current fps: %.01lf", 1 / frame_int);

            if (num_clients) {
                min_client_frame_int = clients[0]->frame_int;
                //min_client_frame_int = 10;
                
                for (int i = 0; i < num_clients; i++) {
                    if (clients[i]->frame_int < min_client_frame_int) {
                        min_client_frame_int = clients[i]->frame_int;
                    }
                }

                frame_int_adj = (min_client_frame_int - frame_int) * 1000000;
                if (frame_int_adj > 0) {
                    //printf("input frame int.: %.0lf us, client frame int.: %.0lf us, frame int. adjustment: %.0lf us",
                            //frame_int * 1000000, min_client_frame_int * 1000000, frame_int_adj);

                    /* sleep between 1000 and 50000 us, depending on the frame interval adjustment */
                    int min_time = 4 * frame_int_adj < 50000 ? 4 * frame_int_adj : 50000;
                    int max_sleep_time = 1000 > min_time ? 1000 : min_time;
                    usleep(max_sleep_time);
                }
            }

            /* check for incoming clients;
             * placing this code inside the if (sep) will simply
             * reduce the number of times we check for incoming clients,
             * with no particular relation to the frame separator we've just found */
             
            client_t *client = NULL;

            if (!max_clients || num_clients < max_clients) {
                client = wait_for_client(server_fd);
            }          

            if (client) {
                if (pthread_create(&client->thread, NULL, (void *(*) (void *)) handle_client, client)) {
                    printf("pthread_create() failed");
                    return -1;
                }

                if (pthread_mutex_lock(&clients_mutex)) {
                    printf("pthread_mutex_lock() failed");
                    return -1;
                }

                clients = realloc(clients, sizeof(client_t *) * (num_clients + 1));
                clients[num_clients++] = client;
                printf("Total clients now after accept: %d \n", num_clients);

                if (pthread_mutex_unlock(&clients_mutex)) {
                    printf("pthread_mutex_unlock() failed");
                    return -1;
                }
            }
        }

	}
	
	running = 0;

    printf("closing server \n");
    close(server_fd);

    printf("waiting for clients to finish \n");
    for (int i = 0; i < num_clients; i++) {
        clients[i]->jpeg_ready = 1;
    }
    if (pthread_cond_broadcast(&jpeg_cond)) {
        printf("pthread_cond_broadcast() failed \n");
        return -1;
    }

    for (int i = 0; i < num_clients; i++) {
        pthread_join(clients[i]->thread, NULL);
    }

    if (pthread_mutex_destroy(&clients_mutex)) {
        printf("pthread_mutex_destroy() failed \n");
        return -1;
    }
    if (pthread_mutex_destroy(&jpeg_mutex)) {
        printf("pthread_mutex_destroy() failed \n");
        return -1;
    }
    if (pthread_cond_destroy(&jpeg_cond)) {
        printf("pthread_cond_destroy() failed \n");
        return -1;
    }
	return 0; 
} 

double get_now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

client_t *wait_for_client(int socket_fd) {
	//printf("wait_for_client: %d \n", 100);
    struct sockaddr_in client_addr;
    unsigned int client_len = sizeof(client_addr);

	//printf("inside wait_for_client 001 \n");
    /* wait for a connection */
    int stream_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_len);
    //we stuck at accept, need unblock O_NONBLOCK
    //https://stackoverflow.com/questions/30733924/server-program-gets-stuck-at-accept-function/30734811
    if (stream_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            printf("accept() failed");
        }
        return NULL;
    }

    /* set socket timeout */
    
    struct timeval tv;

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    setsockopt(stream_fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval));
    setsockopt(stream_fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof(struct timeval));
    
    /* create client structure */
    client_t *client = malloc(sizeof(client_t));
    if (!client) {
        printf("malloc() failed");
        return NULL;
    }

    memset(client, 0, sizeof(client_t));

    client->stream_fd = stream_fd;
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client->addr, INET_ADDRSTRLEN);
    client->port = ntohs(client_addr.sin_port);

    printf("new client connection from %s:%d \n", client->addr, client->port);

    return client;
}

void cleanup_client(client_t *client) {
    //DEBUG_CLIENT(client, "cleaning up");
    printf("[cleanup_client] Total client now is: %d \n", num_clients);
    printf("[cleanup_client] cleaning up client: %s:%d \n", client->addr, client->port);

    if (pthread_mutex_lock(&clients_mutex)) {
        printf("pthread_mutex_lock() failed \n");
    }

    int i, j;
    for (i = 0; i < num_clients; i++) {
        if (clients[i] == client) {
            /* move all further entries back with one position */
            for (j = i; j < num_clients - 1; j++) {
                clients[j] = clients[j + 1];
            }
            break;
        }
    }
    //shutdown(client->stream_fd, SHUT_RDWR);
    close(client->stream_fd);

    if (client->jpeg_tmp_buf) {
        free(client->jpeg_tmp_buf);
    }
    free(client);
    num_clients = num_clients - 1;
    clients = realloc(clients, sizeof(client_t *) * (num_clients));

    if (pthread_mutex_unlock(&clients_mutex)) {
        printf("pthread_mutex_unlock() failed \n");
    }
    printf("Total client after cleanning is: %d \n", num_clients);
}



void handle_client(client_t *client) {
    //DEBUG_CLIENT(client, "reading client request");
    printf("reading client request \n");
    int result = read_request(client);
    printf(">>>>>> read_request result: %d \n", result);
    if (result < 0) {
        //ERROR_CLIENT(client, "failed to read client request");
        printf("failed to read client request \n");
        cleanup_client(client);
        
        return;
    }
    //DEBUG_CLIENT(client, "writing response header");
    printf("writing response header \n");
    result = write_response_ok_header(client);
    if (result < 0) {
        //ERROR_CLIENT(client, "failed to write response header");
        printf("failed to write response header \n");
        cleanup_client(client);
        
        return;
    }
    client->last_frame_time = get_now();

    while (running) {
        if (pthread_mutex_lock(&jpeg_mutex)) {
            //ERROR_CLIENT(client, "pthread_mutex_lock() failed");
            printf("pthread_mutex_lock() failed \n");
            break;
        }
        while (!client->jpeg_ready) {
			int cond_wait_result = 1;
			cond_wait_result = pthread_cond_wait(&jpeg_cond, &jpeg_mutex);
            if (cond_wait_result) {
                //ERROR_CLIENT(client, "pthread_mutex_wait() failed");
                printf("pthread_mutex_wait() failed");
                pthread_mutex_unlock(&jpeg_mutex);
                break;
            }
        }
        
        /* copy the jpeg buffer into the client's temporary buffer,
         * but first make sure there's enough space */
        if (jpeg_size > client->jpeg_tmp_buf_max_size) {
            //DEBUG_CLIENT(client, "temporary buffer increased to %d bytes", jpeg_size);
            //printf("temporary buffer increased to %d bytes \n", jpeg_size);
            client->jpeg_tmp_buf_max_size = jpeg_size;
            client->jpeg_tmp_buf = realloc(client->jpeg_tmp_buf, client->jpeg_tmp_buf_max_size);
        }

        client->jpeg_tmp_buf_size = jpeg_size;
        memcpy(client->jpeg_tmp_buf, jpeg_buf, client->jpeg_tmp_buf_size);

        if (pthread_mutex_unlock(&jpeg_mutex)) {
            //ERROR_CLIENT(client, "pthread_mutex_unlock() failed");
            printf("pthread_mutex_unlock() failed \n");
        }

        double now = get_now();
        client->frame_int = client->frame_int * 0.7 + (now - client->last_frame_time) * 0.3;
        client->last_frame_time = now;
        //DEBUG_CLIENT(client, "current fps: %.01lf", 1 / client->frame_int);
        //printf("current fps: %.01lf \n", 1 / client->frame_int);

        /* clear the ready flag for this client */
        client->jpeg_ready = 0;

        if (!running) {
			printf("handle_client while loop get here NOT RUNNING \n");
            break; /* speeds up the shut down procedure a bit */
        }

        //DEBUG_CLIENT(client, "writing multipart header");
        //printf("writing multipart header \n");
        result = write_multipart_header(client, client->jpeg_tmp_buf_size);
        if (result < 0) {
            //ERROR_CLIENT(client, "failed to write multipart header");
            printf("failed to write multipart header \n");
            break;
        }
        else if (result == 0) {
            //INFO_CLIENT(client, "connection closed");
            printf("connection closed after write_multipart_header \n");
            break;
        }

        //DEBUG_CLIENT(client, "writing jpeg data (%d bytes)", client->jpeg_tmp_buf_size);
        result = write_to_client(client, client->jpeg_tmp_buf, client->jpeg_tmp_buf_size);
        if (result < 0) {
            //ERROR_CLIENT(client, "failed to write jpeg data");
            printf("failed to write jpeg data \n");
            break;
        }
        else if (result == 0) {
            //INFO_CLIENT(client, "connection closed");
            printf("connection closed after write_to_client jpeg \n");
            break;
        }
    }
    cleanup_client(client);
}

int read_request(client_t *client){
	char buf[REQ_BUF_LEN];
    char *line_end, *header_mid;
    char *header_name, *header_value;
    //char *auth_mode, *auth_basic_hash;
    char *strtok_ptr;
    int size, found, offs = 0;

    memset(buf, 0, REQ_BUF_LEN);

    while (running) {
        if (offs >= REQ_BUF_LEN) {
            //ERROR_CLIENT(client, "request header too large");
            return -1;
        }

        size = read(client->stream_fd, buf + offs, REQ_BUF_LEN - offs);
        if (size < 0) {
            if (errno == EAGAIN) {
                //ERROR_CLIENT(client, "timeout reading from client");
                printf("timeout reading from client %s:%d \n", client->addr, client->port);
                return -1;
            }
            else if (errno == EINTR) {
                break;
            }
            else {
				printf("read() failed from client %s:%d \n", client->addr, client->port);
                //ERRNO_CLIENT(client, "read() failed");
                return -1;
            }
        }
        else if (size == 0) {
            //ERROR_CLIENT(client, "connection closed");
            printf("connection closed from client %s:%d \n", client->addr, client->port);
            return -1;
        }

        offs += size;

        line_end = strstr(buf, "\r\n\r\n");
        if (line_end) {
            /* two new lines end the request */
            line_end[4] = 0;
            break;
        }
    }

    //DEBUG_CLIENT(client, "received request header");

    offs = 0; //this offs=0 is neccessary
    while (running && (line_end = strstr(buf + offs, "\r\n"))) {
        if (offs == 0) { /* first request line */
            found = sscanf(buf, "%9s %1023s %9s", client->method, client->uri, client->http_ver);
            if (found != 3) {
                //ERROR_CLIENT(client, "invalid request line");
                return -1;
            }

            //DEBUG_CLIENT(client, "%s %s %s", client->method, client->uri, client->http_ver);
        }
        else { /* subsequent line, request header */
            header_mid = strstr(buf + offs, ": ");
            if (header_mid && header_mid < line_end) {
                header_name = strndup(buf + offs, header_mid - buf - offs);
                header_mid += 1; /* skip ":" */
                while (header_mid < line_end && *header_mid == ' ') {
                    header_mid++; /* skip header value leading spaces */
                }
                header_value = strndup(header_mid, line_end - header_mid);

                
                free(header_name);
                free(header_value);
            }
        }

        offs = line_end - buf + 2;
    }
    printf("read_request from client %s:%d \n", client->addr, client->port);
    //DEBUG_CLIENT(client, "request read");
    return 0;
}

int write_to_client(client_t *client, char *buf, int size) {
	int written = write(client->stream_fd, buf, size);

    if (written < 0) {
        if (errno == EPIPE || errno == EINTR) {
			//ERRNO_CLIENT(client, "write() failed errno == EPIPE || errno == EINTR");
			printf("write() failed errno == EPIPE || errno == EINTR client: %s:%d \n", client->addr, client->port);
            return 0;
        }
        else {
            //ERRNO_CLIENT(client, "write() failed");
            printf("write() failed client: %s:%d \n", client->addr, client->port);
            
            return -1;
        }
    }
    else if (written < size) {
        //ERROR_CLIENT(client, "not all data could be written");
        printf("not all data could be written \n");
        return -1;
    }
    else if(written == 0){
		printf("write_to_client: 000 \n");
		return 0;
	}
    return written;
}

int write_multipart_header(client_t *client, int jpeg_size) {
    static int multipart_header_len = 0;
    if (!multipart_header_len) {
        multipart_header_len = strlen(MULTIPART_HEADER);
    }
    int written = write_to_client(client, (char *) MULTIPART_HEADER, multipart_header_len);
    if (written <= 0) {
        return written;
    }
    char size_str[16];
    snprintf(size_str, 16, "%d\r\n\r\n", jpeg_size);
    return write_to_client(client, size_str, strlen(size_str));
}

int write_response_ok_header(client_t *client) {
    char *data = malloc(strlen(RESPONSE_OK_HEADER_TEMPLATE) + 16);
    sprintf(data, RESPONSE_OK_HEADER_TEMPLATE, SOFTWARE_VERSION);
    int r = write_to_client(client, data, strlen(data));
    free(data);
    return r;
}

/*
int init_server() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        //ERRNO("socket() failed");
        return -1;
    }

    int tr = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(tr)) < 0) {
        //ERRNO("setsockopt() failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        //ERRNO("bind() failed");
        close(socket_fd);
        return -1;
    }

    if (listen(socket_fd, 5) < 0) {
        //ERRNO("listen() failed");
        close(socket_fd);
        return -1;
    }

    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0) {
        //ERRNO("fcntl() failed");
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}
*/

void bye_handler(int signal) {
    if (!running) {
        //INFO("interrupt already received, ignoring signal");
        printf("interrupt already received, ignoring signal");
        return;
    }

    //INFO("interrupt received, quitting");
    printf("interrupt received, quitting");
    running = 0;
}

char *str_timestamp() {
    static __thread char s[20];

    time_t t = time(NULL);
    struct tm *tmp = localtime(&t);

    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", tmp);
    return s;
}

