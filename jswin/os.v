module port_os
import os

pub fn del_file(cname charptr)int{
	os.rm(tos3(cname))
	return 0
}
