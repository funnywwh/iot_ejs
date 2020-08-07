int platform_udp_open();
void platform_udp_close(int sock);
int platform_udp_sendto(int sock,const void* data,int size,const char* dst,int port);
int platform_udp_recvfrom(int sock,void* buf,int size,const char* dst,int port);
int platform_socket_wait_recv(int sock,int ms);
//可以通过close async_sock来中断sock的等待
int platform_socket_wait_recv_for_async(int sock,int async_sock,int ms);