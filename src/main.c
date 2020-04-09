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

#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>

#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
using namespace std;

#define PORT 8080

void setHttpHeader(char httpHeader[]);
void setHttpHeader_other(char httpHeader[], char *path);
void report(struct sockaddr_in *serverAddress);
char* parse(char line[]);
char* parse1(char line[]);
std::string read_image(const std::string& image_path);
int send_image(int & fd, std::string& image);
int send_image1(int & fd, char image_path[]);


//https://stackoverflow.com/questions/45670369/c-web-server-image-not-showing-up-on-browser
char imageheader[] = 
"HTTP/1.1 200 Ok\r\n"
"Content-Type: image/jpeg\r\n\r\n";


int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    char httpHeader[100000] = "HTTP/1.1 200 OK\r\n\n";
    //char httpHeader1[800000] = "HTTP/1.1 200 OK\r\n\n";
    
    
    
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
    int write_ok;
    
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
        
        char httpHeader1[800000] = "HTTP/1.1 200 OK\r\n\n";

        char *parse_string = parse(buffer);  //Try to get the path which the client ask for
        printf("Client ask for path: %s\n", parse_string);

        //prevent strtok from changing the string
        //https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
        char *copy = (char *)malloc(strlen(parse_string) + 1);
        strcpy(copy, parse_string);
        char *parse_ext = parse1(copy);  // get the file extension such as JPG, jpg

        if(strlen(parse_string) <= 1){
            //case that the parse_string = "/"  --> Send index.html file
            write(new_socket , httpHeader , strlen(httpHeader));
        }
        else if ((parse_ext[0] == 'j' && parse_ext[1] == 'p' && parse_ext[2] == 'g') || (parse_ext[0] == 'J' && parse_ext[1] == 'P' && parse_ext[2] == 'G'))
        {
            //send image to client
            char path_head[500] = ".";
            strcat(path_head, parse_string);
            //std::string img = read_image(path_head);
            //write_ok = send_image(new_socket, img);

            send_image1(new_socket, path_head);
        }
        else{
            //send other file such as .css, .html and so on
            setHttpHeader_other(httpHeader1, parse_string);
            write(new_socket , httpHeader1 , strlen(httpHeader1));
            printf("\n Send other file, total size: %d \n", strlen(httpHeader1));
        }
        printf("------------------Server sent----------------------------------------------------\n");
        close(new_socket);
    }
    return 0;
}

void setHttpHeader(char httpHeader[])
{   
    // File object to return
    
    FILE *htmlData = fopen("index.html", "r");

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
    //printf("\n Length of httpheader: %d", strlen(httpHeader));
    
    FILE *htmlData1 = fopen(path_head, "r");

    ////char httpHeader1[8000] = "HTTP/1.1 200 OK\r\n\n";
    int n = 0;
    int size_data = 800000;
    char line[100];

    char *responseData;
    responseData = (char*)malloc(size_data * sizeof(char));  
    //https://stackoverflow.com/questions/5099669/invalid-conversion-from-void-to-char-when-using-malloc/5099675 

    if(htmlData1){
        
        while (fgets(line, 100, htmlData1) != 0 ) {
            strcat(responseData, line);
            n++;
        }
        strcat(httpHeader, responseData);
        fclose(htmlData1);
        printf("\n read %d time and Length of file: %d", n, strlen(responseData));
        //free(responseData);      
    }
    else
    {
        printf("\n Read other file problem");
    }
    //delete[] responseData;
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
    char *message;
    char * token = strtok(line, " ");
    int current = 0;

    while( token != NULL ) {
      
      token = strtok(NULL, " ");
      if(current == 0){
          message = token;
          return message;
      }
      current = current + 1;
      
   }
   printf("arrive here");
   return message;
    
}

char* parse1(char line[])
{
    char *message;
    char * token = strtok(line, ".");
    int current = 0;

    while( token != NULL ) {
      
      token = strtok(NULL, " ");
      if(current == 0){
          message = token;
          return message;
      }
      current = current + 1;
      
   }
   printf("arrive here");
   return message;
    
}

std::string read_image(const std::string& image_path){
    std::ifstream is(image_path.c_str(), std::ifstream::in);
    is.seekg(0, is.end);
    int flength = is.tellg();
    is.seekg(0, is.beg);
    char * buffer = new char[flength];
    is.read(buffer, flength);
    std::string image(buffer, flength);
    return image;
}

int send_image(int & fd, std::string& image){

    int body_length = image.size();
    const char * body = image.data();
    int response_length = body_length;
    char * buffer = new char[response_length];
    memcpy(buffer, body, body_length);

    int ret = write(fd, buffer, response_length);

    delete [] buffer;
    return ret;
}


//https://stackoverflow.com/questions/45670369/c-web-server-image-not-showing-up-on-browser
//http://www.tldp.org/LDP/LG/issue91/tranter.html
//https://linux.die.net/man/2/fstat
int send_image1(int & fd, char image_path[]){

    /*
    char imageheader[] = 
    "HTTP/1.1 200 Ok\r\n"
    "Content-Type: image/jpeg\r\n\r\n";
    */
    struct stat stat_buf;  /* hold information about input file */

    write(fd, imageheader, sizeof(imageheader) - 1);

    int fdimg = open(image_path, O_RDONLY);
    fstat(fdimg, &stat_buf);
    int sent = sendfile(fd, fdimg, NULL, stat_buf.st_mode);
    close(fdimg);

   
}

