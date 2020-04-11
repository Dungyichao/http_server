# Simple Http Web Server in Linux using C/C++ from Scratch
In this tutorial, we will demonstrate how to build a http web server from scratch without using any third party library. Please go through a good reading resource first: 
[Medium link](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
, or you can read from the pdf version provided in this tutorial (
[pdf link](https://github.com/Dungyichao/http_server/blob/master/doc/HTTP%20Server_%20Everything%20you%20need%20to%20know%20to%20Build%20a%20simple%20HTTP%20server%20from%20scratch_pdf2.pdf)
). The tutorial in the Medium post only gives you a abstract concept and simple implementation but the author doesn't finish it. 

<b>My tutorial will show you how to make a fully functional web server in 200 lines of code. </b> <br />

# 1. Basic Knowledge <br />
In the internet world, there is always a server who can serve multiple clients. For example, Google, Netflix, Facebook... and so on are servers. People like us are client and we can use web browser (Chrome, Edge, Opera, Firefox....) to communicate with servers. <br />

<p align="center">
<img src="/img/server-client-role.JPG" height="100%" width="100%">  
</p>

You can make a web server at your home and use your own laptop to access the server through LAN which stands for Local Area Network (having a Wi-Fi router can create a LAN at your home). However, if your html file includes some resources in WAN (Wide Area Network), then you need to be able to access the internet for displaying your html correctly.

What you need is at least one PC or laptop (acts as server) running in Linux (Ubuntu or Debian) which should connect to the router. You can use cell phone as a client.  

# 2. Overview
We are going to implement code for a http server on Ubuntu Desktop. Please follow the Visual Studio Code official website to create a project (
[link](https://code.visualstudio.com/docs/cpp/config-linux)
). Download the web content (Demo website: https://npcasc2020.firebaseapp.com/)  which provide to you in this tutorial folder src (
[link](https://github.com/Dungyichao/http_server/blob/master/src/Web_Content_1.zip)
). Unzip the content and put all the content into your code project. It will be like the following image.

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

I made this website (host on 
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

The story is, the server keep listening any message it received, then we need to analyze what the useful information in the message by parsing it. The useful information we care about is the file name (with path) and file extension. The server then open the file according to the path and put the content of the file into a reply-message which we will later send to the client. Before sending the reply-message, we should first tell the client what kind of file content type we are going to send, maybe image file (.jpg, .png, ...) or txt file (.html, .doc, ...) and so on (refer to the link: 
[link](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types)
), then we can send the reply-message (content of file) to the client. 

# 3. Implement the Code
The overall code can be viewed from the following link: https://github.com/Dungyichao/http_server/blob/master/src/helloworld.cpp

After running the code, and then enter the ip address and port number in the web browser, you will see the following animation in your terminal
<p align="center">
<img src="/img/runcode.gif" height="90%" width="90%">  
</p>

## 3.1 Code Structure
We keep looping through the following code in sequence, namely 1 --> 2 --> 3 --> 4 --> 5 --> 1 --> 2 --> ......  We only focus on number 3 and number 4 and the reply function as well. <br />

<p align="center">
<img src="/img/code_struct.JPG" height="90%" width="90%">  
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
OK! I think you nail it.


## 3.3 Classify the Request 

## 3.4 Reply to the Client
