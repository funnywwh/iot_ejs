const net = require('net');
const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const port = new SerialPort("COM4", { baudRate: 115200 },function(err){
    console.debugf(err);
    if(err){
        port = null;
    }
})

// // 指定连接的tcp server ip，端口
// var options = {
//     host : '192.168.1.42',  
//     port : 9091
// }

// var tcp_client = net.Socket();

// // 连接 tcp server
// tcp_client.connect(options,function(){
//     console.debugf('connected to Server');
// })

// // 接收数据
// tcp_client.on('data',function(data){
//     console.debugf('received data: %s from server', data.toString());
// })

// tcp_client.on('end',function(){
//     console.debugf('data end!');
// })

// tcp_client.on('error', function () {
//     console.debugf('tcp_client error!');
// })

const parser = new Readline()
port.pipe(parser)



var tcp_server = net.createServer();  // 创建 tcp server
// 监听 端口
tcp_server.listen(9091,function (){
    console.debugf('tcp_server listening 9091');
});

// 处理客户端连接
tcp_server.on('connection',function (socket){
    console.debugf(socket.address());
    DealConnect(socket)
})

tcp_server.on('error', function (){
    console.debugf('tcp_server error!');
})

tcp_server.on('close', function () {
    console.debugf('tcp_server close!');
})
const hexchars = "01234567890abcdef";
function from_hex(buf){
	let b = 0;
    let z = 0;
    let hexcharslen = hexchars.length;
    let outbuf = [];
    let size = buf.length;
	for(let i = 0;i<size;i++){
		for(let j = 0;j<hexcharslen;j++){
			if(buf[i] == hexchars[j]){
				if(z == 0){
					z = 1;
					b = j<<4;
				}else{
					z = 0;
					b = b | j;
					outbuf.push(b);
				}
				break;
			}
		}
	}
	return outbuf;
}

function to_hex(inbuf){
    let outbuf = [];
	for(let i = 0;i<inbuf.length;i++){
		let b = inbuf[i];
		let j = i*2;
		outbuf.push(hexchars[b&0xf]);
		outbuf.push(hexchars[(b>>4)&0xf]);
	}
	return outbuf;
}
// 处理每个客户端消息
function DealConnect(socket){
    //     function recv(data){
    //         if(socket){
    //             let readstr = data.toString();
                
    //             // if(readstr.indexOf('debug:') < 0){
    //                 console.debugf('from target:'+readstr);
    //                 socket.write(data);
    //             // }else{
    //             //     console.debugf('logger  :'+readstr);
    //             // }
    //         }
    //     }
    //     if(tcp_client){
    //         tcp_client.on('data',recv);    
    //     }
        
    //     socket.on('data',function(data){
    //         console.debugf('to target:'+data.toString());
    //         if(tcp_client){            
    //             tcp_client.write(data);
    //         }
    //     })

    //     // 客户端正常断开时执行
    //     socket.on('close', function () {
    //         console.debugf('client disconneted!');
    //         // port && port.off('data',recv);
    //         // tcp_client && tcp_client.close()
    //         tcp_client = null;
    //     })
    // // 客户端正异断开时执行
    //     socket.on("error", function (err) {
    //         console.debugf('client error disconneted!');
    //         // port && port.off('data',recv);
    //         // socket = null;
    //     });        

    

    // port.pipe(socket);
    function recv(data){
        if(socket){
            let readstr = data.toString();
            console.debugf('from target:'+readstr);
            if(readstr.indexOf("l:") == 0){

            }else if(readstr.indexOf("d:") == 0){
                socket.write(out);
            }
        }
    }
    if(parser){
        parser.on('data',recv);    
    }
    
    socket.on('data',function(data){
        console.debugf('to target:'+data.toString());
        if(parser){            
            parser.write(new Uint8Array(to_hex(data)));
        }
    })

    // 客户端正常断开时执行
    socket.on('close', function () {
        console.debugf('client disconneted!');
        parser && parser.off('data',recv);
        socket = null;
    })
    // 客户端正异断开时执行
    socket.on("error", function (err) {
        console.debugf('client error disconneted!');
        // port && port.off('data',recv);
        // socket = null;
    });
}

// port.write('ROBOT POWER ON\n')
//> ROBOT ONLINE