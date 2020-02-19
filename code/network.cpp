#include <winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>

struct network_data
{
    addrinfo *res;
    int Socket;
    sockaddr_in *Remote;
};

void StartUpNetwork()
{
    WSADATA wsaData;   // if this doesn't work
    //WSAData wsaData; // then try this instead
    
    // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0:
    
    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }
    
}

void EndDownNetwork()
{
    WSACleanup();
}


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int GetIPAddress(addrinfo **res, char *port, char **PrintOut)
{
    struct addrinfo hints;
    int status;
    char ipstr[46];
    
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE; 
    
    if ((status = getaddrinfo(NULL, port, &hints, res)) != 0)
    {
        sprintf(PrintOut[0], "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }
    
    for(addrinfo *p = *res;p != NULL; p = p->ai_next)
    {
        void *addr;
        char *ipver;
        
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) 
        { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else
        { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        
        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        sprintf(PrintOut[1],"  %s: %s\n", ipver, ipstr);
    }
    
    freeaddrinfo(*res); // free the linked list
    return 0;
}

int CreateSocket(addrinfo *res, char *PrintOut)
{
    if(res)
    {
        int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        sprintf(PrintOut, "socket #: %i\n", sockfd);
        
        
        return sockfd;
    }
    else 
    {
        return 0;
    }
}

int FindConnection(sockaddr_in *connection, char *port)
{
    int sendtoport = atoi(port);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    // TODO(Barret5Ocal): be able to add in custom ip
    addr.sin_addr.s_addr = htonl(( 127 << 24 ) |
                                 ( 0 << 16 ) |
                                 ( 0 << 8  ) |
                                 1);
    
    addr.sin_port = htons(sendtoport);
    
    *connection = addr;
    
    return 1;
}

int SendMessage(char *Message, int sockfd, sockaddr *addr)
{
    printf("sending %s", Message);
    
    //char *msg = "Beej was here!";
    int len, bytes_sent;
    len = strlen(Message);
    bytes_sent= sendto(sockfd, Message, len, 0, (sockaddr*)&addr,
                       sizeof(sockaddr_in));
    
    
    return 1; 
    
}

int Recieve(int sockfd, char *PrintOut[3])
{
    
#define MAXBUFLEN 100
    char buf[MAXBUFLEN];
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;
    int numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
                            (struct sockaddr *)&their_addr, &addr_len);
    
    if(numbytes > 0)
    {
        char s[INET6_ADDRSTRLEN];
        
        sprintf(PrintOut[0], "listener: got packet from %s\n",
                inet_ntop(their_addr.ss_family,
                          get_in_addr((struct sockaddr *)&their_addr),
                          s, sizeof s));
        sprintf(PrintOut[1], "listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        sprintf(PrintOut[2], "listener: packet contains \"%s\"\n", buf);
    }
    
    return 1;
}