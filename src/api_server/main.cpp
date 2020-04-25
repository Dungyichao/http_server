#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include "json.hpp"
#include <arpa/inet.h> //https://www.systutorials.com/docs/linux/man/0p-arpa_inet/
//https://github.com/nlohmann/json
using json = nlohmann::json;


#define PORT 8081

char* parse(char line[], const char symbol[]);
int send_message(int fd, char image_path[], char head[]);


//https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
//https://stackoverflow.com/questions/45670369/c-web-server-image-not-showing-up-on-browser

char http_header[25] = "HTTP/1.1 200 Ok\r\n";


int main(int argc, char const *argv[])
{
	//json j;
	int kkk = 33;
	json j2 = {
		{"pi", 3.141},
		{"happy", true},
		{"name", "Niels"},
		{"nothing", nullptr},
		{"answer", {
			{"everything", kkk}
		}},
		{"list", {1, 0, 2}},
			{"object", {
			{"currency", "USD"},
			{"value", 42.99}
		}}
	};
	int server_fd, new_socket;
	long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In sockets");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
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
	//setHttpHeader(httpHeader);
	//int write_ok;

	while (1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		//https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c
		struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
		struct in_addr ipAddr = pV4Addr->sin_addr;
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);  //#include <arpa/inet.h>
		printf("Client IP: %s", str);

		/* IPv6
		struct sockaddr_in6* pV6Addr = (struct sockaddr_in6*)&client_addr;
		struct in6_addr ipAddr = pV6Addr->sin6_addr;
		char str[INET6_ADDRSTRLEN];
		inet_ntop( AF_INET6, &ipAddr, str, INET6_ADDRSTRLEN );
		*/


		char buffer[30000] = { 0 };
		valread = read(new_socket, buffer, 30000);

		printf("\n%s \n ", buffer);

		//char httpHeader1[800021] = "HTTP/1.1 200 OK\r\n\n";

		char *parse_string = parse(buffer, " ");  //Try to get the path which the client ask for
		printf("Client ask for path: %s\n", parse_string);

		//prevent strtok from changing the string
		//https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
		//https://stackoverflow.com/questions/5099669/invalid-conversion-from-void-to-char-when-using-malloc/5099675 
		char *copy = (char *)malloc(strlen(parse_string) + 1);
		strcpy(copy, parse_string);
		char *parse_ext = parse(copy, ".");  // get the file extension such as JPG, jpg

		char *copy_head = (char *)malloc(strlen(http_header) + 200);
		strcpy(copy_head, http_header);

		//https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
		if (strlen(parse_string) <= 1) {
			//case that the parse_string = "/"  --> Send index.html file
			//write(new_socket , httpHeader , strlen(httpHeader));
			char s[6] = "Hello";
			char path_head[500] = ".";

			//https://www.techiedelight.com/convert-string-char-array-cpp/
			std::string ss = j2.dump();
			char cstr[ss.size() + 1];
			strcpy(cstr, ss.c_str());

			strcat(copy_head, "Content-Type: application/json\r\n\r\n");
			write(new_socket, copy_head, strlen(copy_head));
			write(new_socket, cstr, strlen(cstr));
			/*
			strcat(path_head, "/index.html");
			strcat(copy_head, "Content-Type: text/html\r\n\r\n");
			send_message(new_socket, path_head, copy_head);
			*/
		}
		else if ((parse_ext[0] == 'j' && parse_ext[1] == 'p' && parse_ext[2] == 'g') || (parse_ext[0] == 'J' && parse_ext[1] == 'P' && parse_ext[2] == 'G'))
		{
			//send image to client
			char path_head[500] = ".";
			strcat(path_head, parse_string);
			strcat(copy_head, "Content-Type: image/jpeg\r\n\r\n");
			send_message(new_socket, path_head, copy_head);
		}
		else if (parse_ext[0] == 'i' && parse_ext[1] == 'c' && parse_ext[2] == 'o')
		{
			//https://www.cisco.com/c/en/us/support/docs/security/web-security-appliance/117995-qna-wsa-00.html
			char path_head[500] = ".";
			strcat(path_head, "/img/favicon.png");
			strcat(copy_head, "Content-Type: image/vnd.microsoft.icon\r\n\r\n");
			send_message(new_socket, path_head, copy_head);
		}
		else if (parse_ext[0] == 't' && parse_ext[1] == 't' && parse_ext[2] == 'f')
		{
			//font type, to display icon from FontAwesome
			char path_head[500] = ".";
			strcat(path_head, parse_string);
			strcat(copy_head, "Content-Type: font/ttf\r\n\r\n");
			send_message(new_socket, path_head, copy_head);
		}
		else if (parse_ext[strlen(parse_ext) - 2] == 'j' && parse_ext[strlen(parse_ext) - 1] == 's')
		{
			//javascript
			char path_head[500] = ".";
			strcat(path_head, parse_string);
			strcat(copy_head, "Content-Type: text/javascript\r\n\r\n");
			send_message(new_socket, path_head, copy_head);
		}
		else if (parse_ext[strlen(parse_ext) - 3] == 'c' && parse_ext[strlen(parse_ext) - 2] == 's' && parse_ext[strlen(parse_ext) - 1] == 's')
		{
			//css
			char path_head[500] = ".";
			strcat(path_head, parse_string);
			strcat(copy_head, "Content-Type: text/css\r\n\r\n");
			send_message(new_socket, path_head, copy_head);
		}
		else if (parse_ext[0] == 'w' && parse_ext[1] == 'o' && parse_ext[2] == 'f')
		{
			//Web Open Font Format woff and woff2
			char path_head[500] = ".";
			strcat(path_head, parse_string);
			strcat(copy_head, "Content-Type: font/woff\r\n\r\n");
			send_message(new_socket, path_head, copy_head);
		}
		else {
			//send other file 
			char path_head[500] = ".";
			strcat(path_head, parse_string);
			strcat(copy_head, "Content-Type: text/plain\r\n\r\n");
			send_message(new_socket, path_head, copy_head);

			printf("Else: %s \n", parse_string);

			//setHttpHeader_other(httpHeader1, parse_string);
			//write(new_socket , httpHeader1 , strlen(httpHeader1));         
		}
		printf("\n------------------Server sent----------------------------------------------------\n");
		close(new_socket);
	}
	return 0;
}

char* parse(char line[], const char symbol[])
{
	char *message;
	char * token = strtok(line, symbol);
	int current = 0;

	while (token != NULL) {

		token = strtok(NULL, " ");
		if (current == 0) {
			message = token;
			return message;
		}
		current = current + 1;
	}
	return message;
}

//https://stackoverflow.com/questions/45670369/c-web-server-image-not-showing-up-on-browser
//http://www.tldp.org/LDP/LG/issue91/tranter.html
//https://linux.die.net/man/2/fstat
//http://man7.org/linux/man-pages/man2/stat.2.html
//http://man7.org/linux/man-pages/man2/sendfile.2.html
int send_message(int fd, char image_path[], char head[]) {
	/*
	char imageheader[] =
	"HTTP/1.1 200 Ok\r\n"
	"Content-Type: image/jpeg\r\n\r\n";
	*/
	struct stat stat_buf;  /* hold information about input file */

	write(fd, head, strlen(head));

	int fdimg = open(image_path, O_RDONLY);

	fstat(fdimg, &stat_buf);
	int img_total_size = stat_buf.st_size;
	int block_size = stat_buf.st_blksize;
	//printf("image block size: %d\n", stat_buf.st_blksize);  
	//printf("image total byte st_size: %d\n", stat_buf.st_size);

	while (img_total_size > 0) {
		if (img_total_size < block_size) {
			sendfile(fd, fdimg, NULL, img_total_size);
		}
		else {
			sendfile(fd, fdimg, NULL, block_size);
		}
		img_total_size = img_total_size - block_size;
	}
	close(fdimg);
}
