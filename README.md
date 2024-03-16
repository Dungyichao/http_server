# Simple Http Web Server in Linux using C/C++ from Scratch
In this tutorial, we will demonstrate how to build a http web server from scratch without using any third party library. Please go through a good reading resource first: 
[Medium link](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
, or you can read from the pdf version provided in this tutorial (
[pdf link](https://github.com/Dungyichao/http_server/blob/master/doc/HTTP%20Server_%20Everything%20you%20need%20to%20know%20to%20Build%20a%20simple%20HTTP%20server%20from%20scratch_pdf2.pdf)
). The tutorial in the Medium post only gives you a abstract concept and simple implementation but the author doesn't finish it. 

<b>My tutorial will show you how to make a fully functional web server in no more than 200 lines of code. </b> I also provide ```Node.js``` javascript code of building a simpler web server in the Summary section. <br />

1. [Basic Knowledge](https://github.com/Dungyichao/http_server/blob/master/README.md#1-basic-knowledge-)
2. [Overview](https://github.com/Dungyichao/http_server/blob/master/README.md#2-overview)
    * 2.1 [System Requirement](https://github.com/Dungyichao/http_server/blob/master/README.md#21-system-requirement) 
    * 2.2 [Process Elements](https://github.com/Dungyichao/http_server/blob/master/README.md#22-process-elements)     
3. [Implement the Code](https://github.com/Dungyichao/http_server/blob/master/README.md#3-implement-the-code)
    * 3.1 [Code Structure](https://github.com/Dungyichao/http_server/blob/master/README.md#31-code-structure)
    * 3.2 [Parse the Request from the Client](https://github.com/Dungyichao/http_server/blob/master/README.md#32-parse-the-request-from-the-client) 
    * 3.3 [Classify the Request](https://github.com/Dungyichao/http_server/blob/master/README.md#33-classify-the-request) 
    * 3.4 [Reply to the Client](https://github.com/Dungyichao/http_server/blob/master/README.md#34-reply-to-the-client)   
    * 3.5 [Create Child Process to Handle Clients](https://github.com/Dungyichao/http_server/blob/master/README.md#35-create-child-process-to-handle-clients)  
4. [Summary (with javascript)](https://github.com/Dungyichao/http_server/blob/master/README.md#4-summary)
5. [Video Streaming Protocols](https://github.com/Dungyichao/http_server/blob/master/README.md#3-implement-the-code)
    * 5.1 [HTTP Live Streaming (HLS)](https://github.com/Dungyichao/http_server/blob/master/README.md#51-http-live-streaming-hls)
        - 5.1.1 [HLS Streaming Project](https://github.com/Dungyichao/http_server/blob/master/README.md#511-hls-streaming-project)
    * 5.2 [MJPEG Streaming](https://github.com/Dungyichao/http_server/blob/master/README.md#52-mjpeg)
        - 5.2.1 [MJPEG Streaming Project](https://github.com/Dungyichao/http_server/blob/master/README.md#521-mjpeg-streaming-project)
6. [Advance Topic](https://github.com/Dungyichao/http_server/blob/master/README.md#6-advance-topic)
    * 6.1 [Web Remote Control Robot](https://github.com/Dungyichao/Web-Remote-Control-Robot)
7. [Make HTML More Organized](https://github.com/Dungyichao/http_server/blob/master/README.md#7-make-html-more-organized)
    * 7.1 [Implement the Code](https://github.com/Dungyichao/http_server/tree/master#71-implement-the-code)
    * 7.2 [Bonus: Bingo Game](https://github.com/Dungyichao/http_server/tree/master#72-bonus-bingo-game)

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
       <tr>
            <td align="center">.m3u8</td>
            <td align="Left">application/vnd.apple.mpegurl</td>
        </tr>
       <tr>
            <td align="center">.ts</td>
            <td align="Left">video/mp2t</td>
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

    int sent_size;

    while(img_total_size > 0){
        if(img_total_size < block_size){
            sent_size = sendfile(fd, fdimg, NULL, img_total_size);            
        }
        else{
            sent_size = sendfile(fd, fdimg, NULL, block_size);
        }       
        printf("%d \n", sent_size);
        img_total_size = img_total_size - sent_size;
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

## 3.5 Create Child Process to Handle Clients
In a real world server, we are not going to reply all connected client with only one process. Our server program will not have good performance when multiple clients connecting to us at once. So we will create child process whenever new client connected. Please read the tutorial [link](https://www.linuxhowtos.org/C_C++/socket.htm) ([PDF](https://github.com/Dungyichao/http_server/blob/master/doc/Sockets%20Tutorial.pdf)) - Enhancements to the server code part. 

We modifiy a little bit code from the tutorial link. Please see the following. We only shows the while loop part.
```c++
while (1)
 {
   newsockfd = accept(server_fd,
               (struct sockaddr *) &cli_addr, &clilen);
   if (newsockfd < 0)
     error("ERROR on accept");
   pid = fork();
   if (pid < 0){
     error("ERROR on fork");
     exit(EXIT_FAILURE);  //We add this part
    }
   if (pid == 0)
   {
     //close(server_fd);    //We omint this part because it would cause error
     ......................................
     This part is parsing the message from client, read path file, write file back to client
     ...
     ....
     .....
     ......................................
     close(new_socket);
     exit(0);
   }
   else{
            printf(">>>>>>>>>>Parent create child with pid: %d <<<<<<<<<", pid);
            close(new_socket);
   }
 } /* end o
```

# 4. Summary
This is a simple, experimental but functional Ubuntu web server. Some error protection method not included. Any advise are welcome. I also want to implment a <b>webcam</b> server sending real-time streaming. 

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

# 5. Video Streaming Protocols
The following are some most common streaming protocols and most widely used in current time. However, we will only focus on the HLS.  
<p align="center">
<table>
    <thead>
        <tr>
            <th align="center">Protocols</th>
            <th align="center">Detail</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">Real-Time Messaging Protocol (RTMP)</td>
            <td align="Left">Today it’s mostly used for ingesting live streams. In plain terms, when you set up your encoder to send your video feed to your video hosting platform, that video will reach the CDN via the RTMP protocol. However, that content eventually reaches the end viewer in another protocol – usually HLS streaming protocol.</td>
        </tr>
        <tr>
            <td align="center">Real-Time Streaming Protocol (RTSP)</td>
            <td align="Left">It is a good choice for streaming use cases such as IP camera feeds (e.g. security cameras), IoT devices (e.g. laptop-controlled drone), and mobile SDKs.</td>
        </tr>
        <tr>
            <td align="center">HTTP Live Streaming (HLS)</td>
            <td align="Left">HLS is the most widely-used protocol today, and it’s robust. Currently, the only downside of HLS is that latency can be relatively high.</td>
        </tr>        
    </tbody>
</table>
</p>

## 5.1 HTTP Live Streaming (HLS)
Reference link: <br />
https://www.cloudflare.com/learning/video/what-is-http-live-streaming/ <br />
https://www.dacast.com/blog/hls-streaming-protocol/

Streaming is a way of delivering visual and audio media to users over the Internet. It works by continually sending the media file to a user's device a little bit at a time instead of all at once. With streaming over HTTP, the standard request-response pattern does not apply. <b>The connection between client and server remains open for the duration of the stream</b>, and the server pushes video data to the client so that the client does not have to request every segment of video data. HLS use TCP (more reliable) rather than UDP (more faster) as trasport protocols. 

First, the HLS protocol chops up MP4 video content into short (10-second) chunks with the .ts file extension (MPEG2 Transport Stream). Next, an HTTP server stores those streams, and HTTP delivers these short clips to viewers on their devices. Some software server creates an M3U8 playlist file (e.g. manifest file) that serves as an index for the video chunks. Some .m3u8 and .ts information can be found in the following link [link1](https://nagendrabandi.com/download-m3u8ts-file/#:~:text=A%20M3U8%20stands%20for%20MP3,Flash%20video%20format%20(flv).&text=ts%20files%20are%20referenced%20in,M3U8%20playlist%20file%20index)([PDF](https://github.com/Dungyichao/http_server/blob/master/doc/How%20to%20download%20a%20m3u8.pdf)), [link2](https://datatracker.ietf.org/doc/html/rfc8216)

HLS is compatible with a wide range of devices and firewalls. However, latency (or lag time) tends to be in the 15-30 second range with HLS live streams. 

### 5.1.1 HLS Streaming Project
There are two way to do this project. Please go to the following [link1]( https://five381.com/blog/2020-03/rpi-camera-rtmp-streaming/)([PDF](https://github.com/Dungyichao/http_server/blob/master/doc/STREAMING%20LIVE%20VIDEO%20WITH%20A%20RASPBERRY%20PI%20CAMERA%20OVER%20RTMP.pdf)) and follow the instructions.
<p align="center">
<img src="/img/rapivid_method.JPG" height="95%" width="95%">  
</p>
Rapivid can output segment video files in local folder, however, in option 1, if not using Nginx, when stdout pipe into GStreamer to generate streaming files, it’s .ts file keep growing which never split into segment. It only generate .m3u8 playlist file when you stop the process. It requires to go through Nginx with rtmp sink to generate proper segment .ts files with playlist .m3u8. So we change to the option 2, which use ffmpeg to generate proper segment .ts files with playlist .m3u8. Finally, we can use our handmade http server to send out the .m3u8 and .ts files from local folder to the client browser for streaming. We shows the steps for option 2 below. <br />

First we create the bash file
```bash
$ sudo nano /usr/local/bin/ffmpeg-rpi-stream
```
Place the following into /usr/local/bin/ffmpeg-rpi-stream. Make it executable. Make sure your http server can access the video location (in the base option). Best way is to put handmade http server, index.html, and these .m3u8, .ts file into same location.
```bash
#!/bin/bash
# /usr/local/bin/ffmpeg-rpi-stream
base="/home/pi/Desktop/http/video"     
cd $base

raspivid -ih -t 0 -b 2097152 -w 1280 -h 720 -fps 30 -n -o - | \
ffmpeg -y \
   -use_wallclock_as_timestamps 1 \   #fix error: ffmpeg timestamps are unset in a packet for stream0. 
    -i - \
    -c:v copy \
    -map 0 \
    -f ssegment \
    -segment_time 1 \
    -segment_wrap 4 \
    -segment_format mpegts \
    -segment_list "$base/s.m3u8" \
    -segment_list_size 1 \
    -segment_list_flags live \
    -segment_list_type m3u8 \
    "$base/s_%08d.ts"

```
The following table shows how your configuration would affect the streaming latency time in our project. (This table is based on our Raspberry Pi, camera and LAN speed)
<p align="center">
<table>
    <thead>
        <tr>
            <th align="center">Segment_time</th>
            <th align="center">Segment_wrap</th>
            <th align="center">Segment_List_Size</th>
            <th align="center">Latency</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">1</td>
            <td align="center">2 ~ 20</td>
            <td align="center">1</td>
            <td align="center">3s ~ 5s</td>
        </tr>
        <tr>
            <td align="center">1</td>
            <td align="center">4</td>
            <td align="center">2</td>
            <td align="center">5s</td>
        </tr>
        <tr>
            <td align="center">1</td>
            <td align="center">20</td>
            <td align="center">2</td>
            <td align="center">6s</td>
        </tr>
        <tr>
            <td align="center">1</td>
            <td align="center">20</td>
            <td align="center">5 ~ 10</td>
            <td align="center">9s</td>
        </tr>
        <tr>
            <td align="center">2</td>
            <td align="center">4</td>
            <td align="center">1</td>
            <td align="center">3s ~ 4s</td>
        </tr>
        <tr>
            <td align="center">4</td>
            <td align="center">4</td>
            <td align="center">1</td>
            <td align="center">7s ~ 8s</td>
        </tr>
        <tr>
            <td align="center">4</td>
            <td align="center">4</td>
            <td align="center">2</td>
            <td align="center">11s</td>
        </tr>
    </tbody>
</table>
</p>

Segment_time means how long the .ts file (video length). Segment_wrap means how many .ts file will be kept. Segment_List_Size means how many .ts records will be kept in the .m3u8 which will affact client playback. Segment_wrap should be larger or equal to Segment_List_Size.

Make it executable:
```bash
$sudo chmod +x /usr/local/bin/ffmpeg-rpi-stream
```
We will create a systemd unit file to manage the gstreamer process. We'll configure it to automatically restart the stream if it goes down.
```bash
$sudo nano /lib/systemd/system/ffmpeg-rpi-stream.service
```
Place the following into /lib/systemd/system/ffmpeg-rpi-stream.service
```bash
[Unit]
Description=RPI FFMpeg RTMP Source

[Service]
Type=simple
ExecStart=/usr/local/bin/ffmpeg-rpi-stream
Restart=on-failure
RestartSec=5s

[Install]
WantedBy=multi-user.target
```
Enable and start the service:
```bash
$sudo systemctl daemon-reload
$sudo systemctl enable ffmpeg-rpi-stream
$sudo systemctl start ffmpeg-rpi-stream
```
Now, you can use browser to watch the stream of the raspberry pi camera. 
If want to stop streaming camera service
```bash
$ ps aux
```
Find the PID (raspivid task)
```bash
$sudo kill -9 <pid>
```
## 5.2 MJPEG
HLS is not a good idea for real-time streaming robot project. The latency is not acceptable if you try to remote control your robot. Therefore, we need someting more real time. MJPEG should be able to meet our requirement. MJPEG is mearly a series of JPEG files. A great open source project using C language called streamEye([link](https://github.com/ccrisan/streameye), [backup](https://github.com/Dungyichao/http_server/blob/master/Project/simple_streameye/streameye-master.zip)), and an online tutorial using Python language ([link](https://randomnerdtutorials.com/video-streaming-with-raspberry-pi-camera/), [PDF](https://github.com/Dungyichao/http_server/blob/master/doc/Tutorial%20-%20Video%20Streaming%20with%20Raspberry%20Pi%20Camera.pdf))  are a good starting point. The following project is based on these two online source.

### 5.2.1 MJPEG Streaming Project
Let's take a look at the system structure of StreamEye. Python will be used to capture JPEG image file, and then output to StreamEye.o for further processing, and then act as a http server waiting client to connect and reply with a series of JPEG data.
<p align="center">
<img src="/img/streameye_system.JPG" height="75%" width="75%">  
</p>
I modify the Python, and C code from StreamEye and make it more simpler (less user option, less function) so that we can have a better understanding of the concept.<br />
<br />

Please go to Project folder ([link](https://github.com/Dungyichao/http_server/tree/master/Project/simple_streameye)) and download ```rasp_test.py``` and ```test001.c``` and put in whatever your project folder in rasperrypi. Use Geany to open test001.c, click on Set Build Command ([reference](https://wiki.geany.org/howtos/configurebuildmenu)) and input like the following
<p align="center">
<img src="/img/set_build_command.JPG" height="50%" width="50%">  
</p>
Click on build. After successfully building the c code, open command prompt, cd to your project folder, enter the following command

```bash
$ python rasp_test.py | ./test001
```

Notice that, in test001.c, we've defined the port to 8084, so you can now open web broswer on other PC (in the same network as raspberrypi) and enter address. In my case, my raspberrypi IP is 172.16.216.206, so I put 172.16.216.206:8084 in my web browser to see the stream. 


<p align="center">
<img src="/img/mjpeg-stream.gif" height="90%" width="90%">  
</p>
In above GIF, the raspberrypi is connected to WIFI while my PC is wired connect to network hub.<br />
<br />

If you have other web server servering an index.html which web page contain the MJPEG streaming, you can put the following html tag inside the index.html. 
```html
<img src="http://172.16.216.206:8084/stream.mjpg" width="320" height="240">
```
In my case, I run the web server and streaming server on the same raspberry pi (same ip, but different port). Both program run at the same time. There is no CROS problem because they are in the same domain.

I made a document of my troubleshooting process and answer in this ([link](https://github.com/Dungyichao/http_server/blob/master/Project/simple_streameye/Problem%20and%20answer%20-%20StreamEye%20-%2020220218.pdf)) talking about SIGPIPE, EPIPE, errno, nonblocking Socket, pthread, realloc(), pointer arithmetic, MJPEG parsing.

# 6. Advance Topic
By using our hand made http server, we can implement some interesting project.

## 6.1 Web Remote Control Robot
https://github.com/Dungyichao/Web-Remote-Control-Robot

# 7. Make HTML More Organized
Recently, I made a new Bingo game in the website for our company's Chinese New Year, and the code is getting bigger and larger. I can no longer maintain all the code in just one index.html or one javascript file. The idea is to spread all components into different HTML files, and import these html files into the main index.html. From the following picture, you can see I organize HTML, javascript, images, css files into its folders.

<p align="center">
<img src="/img/organize_html_folders.JPG" height="90%" width="90%">  
</p>

## 7.1 Implement the Code
You can see the source code in this link ([link](https://github.com/Dungyichao/http_server/blob/master/src/Web_Content_2.zip)). The rough idea can be shown in the following code

This is where Javascript function will load child HTML files into this main index.html.
<p align="center">
<img src="/img/index_head_load.JPG" height="90%" width="90%">  
</p>

This is where child HTML files content will be loaded and displayed. Because the browser will read line by line, so the order in the following will be reflected on the website. Actually, those div tag just act like a place holder, the browser will literally insert all the loaded HTML code into these place holder (reference by the id). However, I cannot move the Navigation bar away to separate HTML file. I do not know the reason and cannot find any solution.
<p align="center">
<img src="/img/index_body_div_id.JPG" height="90%" width="90%">  
</p>

This is where the browser will load all Javascript files
<p align="center">
<img src="/img/index_body_js.JPG" height="90%" width="90%">  
</p>

This is just a regular html code where I remove from original index.html and paste the content into separate html file. The following is just one of the example. You can find the rest in the zip file
<p align="center">
<img src="/img/video_html.JPG" height="90%" width="90%">  
</p>

After doing all of this, our web content displayed the same information, but our index.html is more cleaner and more easy to manage. 

## 7.2 Bonus: Bingo Game
This year 2024, I added another Bingo game which allow user to input their bingo, submit to firebase. The firebase administrator will input the called number online. Use can refresh the browser and see each user's name and their number of lines. 

Player Input Demo
https://github.com/Dungyichao/http_server/assets/25232370/b0b9e9f1-f6b0-4c09-9ed1-115a3963c8e9.mp4

https://user-images.githubusercontent.com/25232370/b0b9e9f1-f6b0-4c09-9ed1-115a3963c8e9.mp4

This is player input page
<p align="center">
<img src="/img/Bingo_user_input.JPG" height="90%" width="90%">  
</p>

This is player score
<p align="center">
<img src="/img/Bingo_score.JPG" height="90%" width="90%">  
</p>

Algorithm to calculate Bingo Game match lines. You can find the code implementation in the src/Web_Content_2.zip js folder Refresh_Bingo_Score.js. 
```html
1. create an empty 1-D array which is the same number as player bingo input (m x m)
2. check player's input, if match, insert 1 into array, otherwise, insert 0 into array
3. Check row by row, column by column, two diagonal. If line added up to m, player matched line plus one
4. Continue to check next player
```
