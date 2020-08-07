#include <winsock.h>
#include "os.h"
#include "log.h"
int platform_socket_wait_recv(int sock,int ms){
    struct timeval tv = {ms/1000,1000*ms%1000}; 
    fd_set         fdread;
    FD_ZERO(&fdread);
    FD_SET(sock, &fdread); 
    select(0, &fdread, NULL, NULL, &tv); 
    if(FD_ISSET(sock, &fdread)){
        return 1;
    }
    return 0;
}

int udp__platform_udp_open();
int platform_udp_open(){
    return udp__platform_udp_open();
}
void udp__platform_udp_close(int sock);
void platform_udp_close(int sock){
    udp__platform_udp_close(sock);
}
int udp__platform_udp_sendto(int sock,const void* data,int size,const char* dst,int port);
int platform_udp_sendto(int sock,const void* data,int size,const char* dst,int port){
    return udp__platform_udp_sendto(sock,data,size,dst,port);
}
int udp__platform_udp_recvfrom(int sock,void* buf,int size,const char* dst,int port);
int platform_udp_recvfrom(int sock,void* buf,int size,const char* dst,int port){
    return udp__platform_udp_recvfrom(sock,buf,size,dst,port);
}
int platform_socket_wait_recv_for_async(int sock,int async_sock,int ms){
    struct timeval timeout = {ms/1000,1000*ms%1000};
    fd_set rdSet;
    FD_ZERO(&rdSet);
	FD_SET(sock, &rdSet);
    FD_SET(async_sock,&rdSet);
	int ret = select(async_sock+1, &rdSet, NULL, NULL, &timeout);
    if(ret > 0){
        if(FD_ISSET(sock, &rdSet)){
            return 1;
        }else if(FD_ISSET(async_sock, &rdSet)){
            return 2;
        }  
    }
 
    return 0;
}