module udp
import net

pub fn platform_udp_open()int{
	s := net.socket_udp() or {
		return 0
	}

	return s.sockfd
}
pub fn platform_udp_close(sock int){
	s := net.Socket{
		sockfd:sock,
	}
	s.close()
}

pub fn platform_udp_sendto(sock int ,data voidptr,size int,address charptr,port int)int{	
	s := net.Socket{
		sockfd:sock,
	}
	s.connect(tos2(address),port)
	sended_size := s.send(data,size) or {
		return 0
	}
	return sended_size
}

pub fn platform_udp_recvfrom(sock int ,buf voidptr,size int,address charptr,port int)int{
	s := net.Socket{
		sockfd:sock,
	}
	s.connect(tos2(address),port)

	return C.recv(s.sockfd, buf, size,0)
}