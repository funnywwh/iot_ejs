#include "iot_socket.h"

int platform_udp_open(){
    int socketfd = socket(OPENAT_AF_INET,OPENAT_SOCK_DGRAM,0);
    if(socketfd < 0 ){
        return socketfd;
    }
    return socketfd;    
}

void platform_udp_close(int sock){
    close(sock);
}

int platform_udp_sendto(int sock,const void* data,int size,const char* dst,int port){
    int send_len = 0;
	struct openat_sockaddr_in udp_server_addr; 

	memset(&udp_server_addr, 0, sizeof(udp_server_addr)); // ��ʼ����������ַ  
    udp_server_addr.sin_family = OPENAT_AF_INET;  
    udp_server_addr.sin_port = htons((unsigned short)port);  
    inet_aton(dst,&udp_server_addr.sin_addr);

    send_len = sendto(sock,data,size, 0,(struct sockaddr*)&udp_server_addr, sizeof(struct openat_sockaddr));
    return send_len;
}

int platform_udp_recvfrom(int sock,void* buf,int size,const char* dst,int port){
    int recv_len;
	openat_socklen_t udp_server_len;

	struct openat_sockaddr_in udp_server_addr; 

	memset(&udp_server_addr, 0, sizeof(udp_server_addr)); // ��ʼ����������ַ  
    udp_server_addr.sin_family = OPENAT_AF_INET;  
    udp_server_addr.sin_port = htons((unsigned short)port);  
    inet_aton(dst,&udp_server_addr.sin_addr);
	udp_server_len = sizeof(udp_server_addr);

    recv_len = recvfrom(sock, buf,size, 0, (struct sockaddr*)&udp_server_addr, &udp_server_len);
    return recv_len;
}
//等待socket 接收信号
int platform_socket_wait_recv(int sock,int ms){
    struct openat_timeval timeout = {ms/1000,1000*ms%1000};
    fd_set rdSet;
    FD_ZERO(&rdSet);
	FD_SET(sock, &rdSet);
    // debugf("time sec:%d usec:%d\n",timeout.tv_sec,timeout.tv_usec);
	int ret = select(sock+1, &rdSet, NULL, NULL, &timeout);
    if(FD_ISSET(sock, &rdSet)){
        return 1;
    }
    return 0;
}

int platform_socket_wait_recv_for_async(int sock,int async_sock,int ms){
    struct openat_timeval timeout = {ms/1000,1000*ms%1000};
    fd_set rdSet;
    FD_ZERO(&rdSet);
	FD_SET(sock, &rdSet);
    FD_SET(async_sock,&rdSet);
    // debugf("time sec:%d usec:%d\n",timeout.tv_sec,timeout.tv_usec);
	int ret = select(sock+2, &rdSet, NULL, NULL, &timeout);
    if(FD_ISSET(sock, &rdSet)){
        return 1;
    }else if(FD_ISSET(async_sock, &rdSet)){
        return 2;
    }
    return 0;
}