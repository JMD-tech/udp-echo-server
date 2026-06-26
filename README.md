# udp-echo-server
An implementation of udp-based echo server and client.  
Basically [Saad Farooq's implementation](https://github.com/BigWheel92/udp-echo-server), with added name resolution and IPv6 support.

# Security disclaimer
Be sure to use only on local private network, or for public network to filter allowed source IPs or only temporary use, 
a publicly accessible UDP echo server could be used as a DDoS relay with spoofed source datagrams.

# How To run:
# First you need to compile client.cpp and server.cpp, and run the server:
  1. Open a terminal in a Linux-based OS.
  2. Compile both files using the command: ```make```
  3. Now run the server using the command: ```./server 8000```
  By default it starts in IPv6 mode which can respond to both IPv6 and IPv4 on the same port.  
  You can add an optional 4 parameter at the end to start in IPv4 mode only but this is normally unnecessary, as default IPv6 mode handles both, and if IPv6 opening fails (like, if IPv6 is disabled in the OS), it fallbacks to IPv4-only mode.

# Now you need to run client.cpp
  4. Open another terminal in linux OS.
  5. Now run the client using the command: ```./client localhost 8000```
  You can add an optional 4 or 6 parameter at the end to force use of IPv4 or IPv6: ```./client localhost 8000 6```  
  Without it, either one which the hostname resolves to will be used, IPv6 by default if hostname resolves to both.  
  Note that sometimes localhost won't resolve to IPv6 ::1, if you don't have it as IPv6 too in /etc/hosts.  
  In this case, you should use (default on most Linux distros) ip6-localhost name instead.

# Some Clarifications:
1. In step 3, the argument 8000 is the port number on which the server will listen. You can pass any valid port number on which you want the server to listen. The server binds to all available ip addresses (INADDR_ANY).

2. The command in step 6 has two command line arguments. The first argument (127.0.0.1 / localhost) is the ip address or hostname of the server. 
If the server is on another machine, then you can pass the ip address or hostname of that machine. The second argument is the port number on which the server is listening.

3. The client will take input from the user and send it to the server. The server will send the input back to the client.

4. When the user passes "exit" as input, the client will terminate.

5. To terminate the server, enter any key and press enter.
