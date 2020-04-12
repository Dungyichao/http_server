# Simple Http Web Server in Linux using C/C++ from Scratch
In this tutorial, we will demonstrate how to build a http web server from scratch without using any third party library. Please go through a good reading resource first: 
[Medium link](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
, or you can read from the pdf version provided in this tutorial (
[pdf link](https://github.com/Dungyichao/http_server/blob/master/doc/HTTP%20Server_%20Everything%20you%20need%20to%20know%20to%20Build%20a%20simple%20HTTP%20server%20from%20scratch_pdf2.pdf)
). The tutorial in the Medium post only gives you a abstract concept and simple implementation but the author doesn't finish it. 

<b>My tutorial will show you how to make a fully functional web server in no more than 200 lines of code. </b> I also provide ```Node.js``` javascript code of building a simpler web server in the Summary section. <br />

[1. Basic Knowledge](https://github.com/Dungyichao/http_server/blob/master/README.md#1-basic-knowledge-)

[2. Overview](https://github.com/Dungyichao/http_server/blob/master/README.md#2-overview)
    [2.1 System Requirement](https://github.com/Dungyichao/http_server/blob/master/README.md#21-system-requirement)
    [2.2 Process Elements](https://github.com/Dungyichao/http_server/blob/master/README.md#22-process-elements)
    
[3. Implement the Code](https://github.com/Dungyichao/http_server/blob/master/README.md#3-implement-the-code)
    [3.1 Code Structure](https://github.com/Dungyichao/http_server/blob/master/README.md#31-code-structure)
    [3.2 Parse the Request from the Client](https://github.com/Dungyichao/http_server/blob/master/README.md#32-parse-the-request-from-the-client)
    [3.3 Classify the Request](https://github.com/Dungyichao/http_server/blob/master/README.md#33-classify-the-request)
    [3.4 Reply to the Client](https://github.com/Dungyichao/http_server/blob/master/README.md#34-reply-to-the-client)
    
[4. Summary](https://github.com/Dungyichao/http_server/blob/master/README.md#4-summary)

# 1. Basic Knowledge <br />
In the internet world, there is always a server who can serve multiple clients. For example, Google, Netflix, Facebook... and so on are servers. People like us are client and we can use web browser (Chrome, Edge, Opera, Firefox....) to communicate with servers. <br />

<p align="center">
<img src="/img/server-client-role.JPG" height="100%" width="100%">  
</p>

You can make a web server at your home and use your own laptop to access the server through LAN which stands for Local Area Network (having a Wi-Fi router can create a LAN at your home). However, if your html file includes some resources in WAN (Wide Area Network), then you need to be able to access the internet for displaying your html correctly.

What you need is at least one PC or laptop (acts as server) running in Linux (
[Ubuntu](https://ubuntu.com/desktop)
or Debian) which should connect to the router. You can use cell phone as a client.  

# 2. Overview
We are going to implement code for a http server on Ubuntu Desktop. Please follow the <b>Visual Studio Code</b> official website to create a project (
[link](https://code.visualstudio.com/docs/cpp/config-linux)
). Download the web content (Demo website: https://npcasc2020.firebaseapp.com/)  which provide to you in this tutorial folder src (
[link](https://github.com/Dungyichao/http_server/blob/master/src/Web_Content_1.zip)
). Unzip the content and put all the content into your code project. It will be like the following image. (Notice that we do all the coding and compiling on Ubuntu Desktop)

<p align="center">
<img src="/img/project_folder.JPG" height="90%" width="90%">  
</p>

Copy paste the code provided in this tutorial (
[link](https://github.com/Dungyichao/http_server/blob/master/src/helloworld.cpp)
) into the helloworld.cpp file. Compile the code and execute it. 

The local ip address of my web server is 172.16.216.205, Subnet Mask is 255.255.0.0, the default gateway should be the ip address of your router , in our case is 172.16.216.6. <b>Modify these number to fit in your case.</b> If everything is working properly, now you can type in <b>172.16.216.205:8080</b> in the browser on your laptop or cellphone (which should connect to Wi-Fi router at your home). What you see in the browser should be the same as the following animation.

<p align="center">
<img src="/img/webpagedemo1.gif" height="95%" width="95%">  
</p>

I made this website (hosted on 
[Google Firebase](https://firebase.google.com/)
) for the activity in our company (Nan Ya Plastics Corp. America which HQ in <b>Taiwan</b>) to celebrate 2020 Chinese New Year. The template is from https://startbootstrap.com/themes/agency/

## 2.1 System Requirement
<p align="center">
<table>
    <thead>
        <tr>
            <th align="center">Role</th>
            <th align="center">Requirement</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">Web Server</td>
            <td align="Left">Linux OS such as <b>Ubuntu</b> or <b>Debian</b>. <br />C\C++ development environment: <b>Visual Studio Code </b>or <b>Geany</b>. (Raspberry pi maybe not a good idea, it can serve the website but it would hang in the middle of transfering large image.)</td>
        </tr>
        <tr>
            <td align="center">Client</td>
            <td align="Left">Whatever OS (Windows, IOS, Android, Ubuntu) which is able to access web browser is good. You can use your cell phone as well.</td>
        </tr>
    </tbody>
</table>
</p>

## 2.2 Process Elements
The following image is basically what we are going to implement in the code. We obmit some initialization part which was mentioned in the Meduim article (
[link](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
), however, you can still find it in our code.
<br />

<p align="center">
<img src="/img/process_element1.jpg" height="90%" width="90%">  
</p>

The story is, the server keep listening any message it received, then we need to analyze what the useful information in the message by parsing it. The useful information we care about is the file name (with path) and file extension. The server then open the file according to the path and put the content of the file into a reply-message which we will later send to the client. Before sending the reply-message, we should first tell the client what kind of file content type we are going to send, maybe image file (.jpg, .png, ...) or txt file (.html, .doc, ...) and so on (refer to https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types), then we can send the reply-message (content of file) to the client. 

# 3. Implement the Code
The overall code can be viewed from the following link: https://github.com/Dungyichao/http_server/blob/master/src/helloworld.cpp

After running the code, and then enter the ip address and port number in the web browser, you will see the following animation in your terminal
<p align="center">
<img src="/img/runcode.gif" height="90%" width="90%">  
</p>

## 3.1 Code Structure
We keep looping through the following code in sequence, namely 1 --> 2 --> 3 --> 4 (a) --> 5 --> 1 --> 2 --> 3 --> 4 (d) --> 5.....  We only focus on number 3 and number 4 and the reply function as well. <br />

<p align="center">
<img src="/img/code_struct.JPG" height="75%" width="75%">  
</p>

## 3.2 Parse the Request from the Client
Let's take a look at what the <b>very first</b> request information the client sends to you 
<p align="center">
<img src="/img/http_request_0.JPG" height="95%" width="95%">  
</p>
At first glance, it contains useless information (maybe not true for Hacker), how about we look at the other request information?
<p align="center">
<img src="/img/http_request_img.JPG" height="95%" width="95%">  
</p>
<p align="center">
<img src="/img/http_request_js.JPG" height="95%" width="95%">  
</p> 
OK ! I think you nail it. The information between GET and HTTP/1.1. That is the file path which the client requires to display the website correctly on it's browser. <br /><br />

The <b>parse function</b> just retrieves the <b>path</b> and <b>file extension</b> (such as .jpg  .html  .css....) from a bunch of information. <br /> 
```c++
char* parse(char line[], const char symbol[])
{
    char *message;
    char * token = strtok(line, symbol);
    int current = 0;

    while( token != NULL ) {
      
      token = strtok(NULL, " ");
      if(current == 0){
          message = token;
          return message;
      }
      current = current + 1;
   }
   return message;
}
```

## 3.3 Classify the Request 
In 
[section 2.2 Process Element](https://github.com/Dungyichao/http_server#22-process-elements)
 we mention that we need to tell the client what kind of content we are going to send in. The classification is just a bunch of <b>if else</b> logic detemination according to the file extension from the <b>parsed</b> information (section 3.2). I just list the partial code in the following to give you some concept.
 
 ```c++
 if(strlen(parse_string) <= 1){
            //case that the parse_string = "/"  --> Send index.html file
            //write(new_socket , httpHeader , strlen(httpHeader));
            char path_head[500] = ".";
            strcat(path_head, "/index.html");
            strcat(copy_head, "Content-Type: text/html\r\n\r\n");
            send_message(new_socket, path_head, copy_head);
}
else if ((parse_ext[0] == 'j' && parse_ext[1] == 'p' && parse_ext[2] == 'g') || 
(parse_ext[0] == 'J' && parse_ext[1] == 'P' && parse_ext[2] == 'G'))
{
            //send image to client
            char path_head[500] = ".";
            strcat(path_head, parse_string);
            strcat(copy_head, "Content-Type: image/jpeg\r\n\r\n");
            send_message(new_socket, path_head, copy_head);
}
else if (parse_ext[strlen(parse_ext)-2] == 'j' && parse_ext[strlen(parse_ext)-1] == 's')
{
            //javascript
            char path_head[500] = ".";
            strcat(path_head, parse_string);
            strcat(copy_head, "Content-Type: text/javascript\r\n\r\n");
            send_message(new_socket, path_head, copy_head);
}
else if (parse_ext[strlen(parse_ext)-3] == 'c' && parse_ext[strlen(parse_ext)-2] == 's' 
&& parse_ext[strlen(parse_ext)-1] == 's')
{
            //css
            char path_head[500] = ".";
            strcat(path_head, parse_string);
            strcat(copy_head, "Content-Type: text/css\r\n\r\n");
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
 ```
 I know you are still wondering the very first request information I mentioned in section 3.2 which contains ```/``` such a useless information. Actually, it does give us a hint to send it our web page, namely ```index.html```. The client will receive the html file looks like the following
 
 <p align="center">
<img src="/img/index_html.JPG" height="95%" width="95%">  
</p>

The client's web browser will read line by line and do whatever the html file tells it. When it reads until line 14 (in above image), the client will send request to the server to ask for ```vendor/fontawesome-free/css/all.min.css``` which is a css file. Server than ```parse``` the request, and then classify the request. 

There are multiple file extension we need to take good care of, the following link shows you a list of file extension: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types . We need to first notify the client what kind of content we are going to send so that the client can receive the content accordingly. The notification message looks like the following:

```c++
HTTP/1.1 200 Ok\r\n
Content-Type: text/html\r\n\r\n
```
You need to replace the ```text/html``` with the proper MIME Type according to the file extension. 

While writing this tutorial, a special file extension request from the client ```/favicon.ico```, however, I couldn't find out the file in my website at all (I also look into all html, css, js files). It turns out that every browser will automatically request for ```/favicon.ico``` which is merely an icon for displaying on the browser tab shown in the following. So what you need is just reply a .ico or .png file to the client.
 <p align="center">
<img src="/img/ico.JPG" height="95%" width="95%">  
</p>

Here we list out some common file extension and their MIME Type.

<p align="center">
<table>
    <thead>
        <tr>
            <th align="center">File Extension</th>
            <th align="center">MIME Type</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">.css</td>
            <td align="Left">text/css</td>
        </tr>
        <tr>
            <td align="center">.html</td>
            <td align="Left">text/html</td>
        </tr>
        <tr>
            <td align="center">.ico</td>
            <td align="Left">image/vnd.microsoft.icon</td>
        </tr>
        <tr>
            <td align="center">.jpg</td>
            <td align="Left">image/jpeg</td>
        </tr>
        <tr>
            <td align="center">.js</td>
            <td align="Left">text/javascript</td>
        </tr>
        <tr>
            <td align="center">.json</td>
            <td align="Left">application/json</td>
        </tr>
        <tr>
            <td align="center">.ttf</td>
            <td align="Left">font/ttf</td>
        </tr>
        <tr>
            <td align="center">.txt</td>
            <td align="Left">text/plain</td>
        </tr>
        <tr>
            <td align="center">.woff</td>
            <td align="Left">font/woff</td>
        </tr>
        <tr>
            <td align="center">.xml</td>
            <td align="Left">text/xml</td>
        </tr>
        <tr>
            <td align="center">.mp3</td>
            <td align="Left">audio/mpeg</td>
        </tr>
        <tr>
            <td align="center">.mpeg</td>
            <td align="Left">video/mpeg</td>
        </tr>
    </tbody>
</table>
</p>
 
## 3.4 Reply to the Client
The following function first send notification message to the client and let it knows what kind of content we are going to send (section 3.3). We then open the file using ```open``` and retrieve information of the file (not the content) using ```fstat``` and store in ```stat object```. Lastly, we read the file content and send the content using ```sendfile```. Because some file might be too large to send in one message, thus, we need to send the content pices by pices (size = block_size).

```c++
int send_message(int fd, char image_path[], char head[]){

    struct stat stat_buf;  /* hold information about input file */

    write(fd, head, strlen(head));

    int fdimg = open(image_path, O_RDONLY);
     
    fstat(fdimg, &stat_buf);
    int img_total_size = stat_buf.st_size;
    int block_size = stat_buf.st_blksize;

    while(img_total_size > 0){
        sendfile(fd, fdimg, NULL, block_size);
        img_total_size = img_total_size - block_size;
    }
    close(fdimg);
}
```
You might not familiar with the above command, so the following link may help you.

<p align="center">
<table>
    <thead>
        <tr>
            <th align="center">Term</th>
            <th align="center">Web Link</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">stat</td>
            <td align="Left">http://man7.org/linux/man-pages/man2/stat.2.html</td>
        </tr>
        <tr>
            <td align="center">sendfile</td>
            <td align="Left">http://man7.org/linux/man-pages/man2/sendfile.2.html <br /> http://www.tldp.org/LDP/LG/issue91/tranter.html</td>
        </tr>
        <tr>
            <td align="center">fstat</td>
            <td align="Left">https://linux.die.net/man/2/fstat</td>
        </tr>
        <tr>
            <td align="center">open</td>
            <td align="Left">http://man7.org/linux/man-pages/man2/open.2.html</td>
        </tr>
    </tbody>
</table>
</p>

# 4. Summary
This is a simple, experimental but functional Ubuntu web server. Some error protection method not inclue. Any advise are welcome. I also want to implment a <b>webcam</b> server sending real-time streaming. 

Is there a simple way? Yes, you can use <b>```Node.js```</b> which is a JavaScript runtime environment where you can build a simple web server in about 60 lines of code. (Youtube Node.js Crash Course: https://www.youtube.com/watch?v=fBNz5xF-Kx4)
```javascript
const http = require('http');
const path = require('path');
const fs = require('fs');

const server = http.createServer((req, res) => {
    let filePath = path.join(__dirname, req.url === '/' ? 'index.html' : req.url);
    let file_extname = path.extname(filePath);
    let contentType = 'text/html';

    switch(file_extname){
        case '.js':
            contentType = 'text/javascript';
            break;
        case '.css':
            contentType = 'text/css';
            break;
        case '.json':
            contentType = 'application/json';
            break;
        case '.png':
            contentType = 'image/png';
            break;
        case '.JPG':
            contentType = 'image/jpg';
            break;
        case '.ico':
            filePath = path.join(__dirname,'favicon.png');
            contentType = 'image/png';
            break;
        case '.ttf':
            contentType = 'font/ttf';
            break;
        case '.woff':
            contentType = 'font/woff';
            break;
        case '.woff2':
            contentType = 'font/woff2';
            break;
    }
    // Read File
    fs.readFile(filePath, (err, content) => {
        if(err){
            if(err.code == 'ENOENT'){
                console.log('Page not found');
            }
            else{
                res.writeHead(500);
                res.end('Server Error: ${err.code}');
            }
        }
        else{
            res.writeHead(200, {'Content-Type': contentType});
            res.end(content);    
        }
    });
});

const PORT = process.env.PORT || 8080;

server.listen(PORT, () => console.log(`Server is running and port is ${PORT}`));
```
