print('poc running');

var cfg = require('app/config');
const MTU = 1300;
const RTP_MASK = 0xc0;
const RTP_HEADER_LEN = 12;
const CMD = {
    CONNECT: 1,
    PING: 2,
    PONG: 3,
    BEGIN_TALK: 4,
    END_TALK: 5,
    OTHER_BEGIN_TALK: 6,
    OTHER_END_TALK: 7,
    //一次req -> resp 用一个seq
    //<SIGNLE_REQ(1byte)><SIGNAL_REQ_ID(1byte)><seq(2byte)><req body len(2byte max MTU)><body offset(2byte)><body>
    SIGNLE_REQ: 8,//信令请求
    //SIGNLE_RSP<1byte><code(1byte)><seq(2byte)><rsp body len(2byte max MTU)><rsp body offset(2byte)><resp body>
    SIGNLE_RSP: 9,//信令应答
    IOT_CONNECT: 20,
    IOT_REGISTER:21,//注册设备:code,imei,version,host,romVersion
    RTP: 0xc0,
};
const SIGNAL_REQ_ID = {
    LOGIN: 1,
    SWITCH_GROUP: 2,//切组
};
const RSP = {
    OK: 0,
    FAILED: 1,
    PARAM_ERROR: 2,//参数错误
    NO_REGISTERED: 3,//未注册设备
    GROUP_NO_EXSIT: 4,//组不存在
    DISABLED: 5,//停用
};
const PT = {
    AUDIO_G711A: 8,
};

var Poc = function(){
    this.udp = new iot.socket.Udp();
    if(this.udp.sock <= 0){
        throw new Error('sock craete error');
    }
    return ;
};

Poc.prototype = {
    connect:function(address,port){
        print("poc connecting")
        var imei = iot.misc.getImei();
        this.imei = imei;
        this.code = imei;
        this.version = '1.0';
        this.rom_version = '1.0';
        var p = {
            code:this.imei,
            appId:'rda8910_jsdk',
            imei:this.imei,
            version:this.version,
            romVersion:this.rom_version,
        };
        var pjson = JSON.stringify(p);
        print(pjson)
        var result = this.request(CMD.IOT_CONNECT,iot.stringToUintArray(pjson),cfg.cmd_retry,cfg.cmd_ms_per_retry);
        print('request result.resp:'+result.resp);
        switch(result.resp){
            case RSP.OK:{
                print('connect ok');
                return ;
            }break;
            case RSP.NO_REGISTERED:{
                this.register();
                return ;
            }break;
            default:{
                var e = new Error("connect unkonwn result:"+result.resp);
                throw e;
            }break;
        }
    },
    register:function(){
        print('registering')
        var param_obj = {
            code:this.code,
            IMEI:this.imei,
            SN:this.imei.substr(-11),
            version:this.version,
            romVersion:this.rom_version,
            host:"",
        };
        var param = JSON.stringify(param_obj);
        print(param);
        var result = this.request(CMD.IOT_REGISTER,iot.stringToUintArray(param),cfg.cmd_retry,cfg.cmd_ms_per_retry);
        print('register.resp:'+result.resp);
        if(result.resp != RSP.OK){
            throw new Error('register failed:'+result.resp);
        }
        this.connect();
    },
    request:function(cmd ,data,retry,ms_per_try){
        print('requesting')
        var param = Uint8Array.allocPlain(data.length+1);
        param[0] = cmd;
        param.set(data,1);
        var out = Uint8Array.allocPlain(MTU);
        for(var i = 0;i<retry;i++){
            var ssize = this.udp.sendto(param,cfg.server_ip,cfg.server_port);
            if(ssize <= 0){
                iot.os.sleep(ms_per_try);
                continue;
            }
            yield()

            var rsize = this.udp.recvfrom(out,cfg.server_ip,cfg.server_port);
            if(rsize < 2){
                print('rsize < 2')
                continue;
            }
            if(out[0] != cmd){
                print('cmd[0]:'+out[0]+" want:"+cmd);
                continue;
            }
            return {
                resp:out[1],
                body:out.subarray(2,rsize),
            }
        }
        throw new Error("timeout");
    },
    send:function(body/**Uint8Array */){
        this.udp.sendto(body,cfg.server_ip,cfg.server_port);
    },
    recv:function(buffer /**Uint8Array */){
        return this.udp.recvfrom(buffer,cfg.server_ip,cfg.server_port);
    },
    ping:function(){
        var reqbody = Uint8Array.allocPlain(1);
        reqbody[0] =CMD.PING;
        this.send(reqbody);
    },
    parse_rtp:function(rtp_pkg){
        if(rtp_pkg.length < RTP_HEADER_LEN){
            throw new Error('invalid rtp package');
        }
        return {
            pt:rtp_pkg[1] & 0x7f,
            end_mask:rtp_pkg[1] & 0x80 > 0 ,
            seq:((rtp_pkg[2]&0xff)<<8)|(rtp_pkg[3]&0xff),
            timestamp:(
                (rtp_pkg[4]<<24) | 
                (rtp_pkg[5]<<16) |
                (rtp_pkg[6]<<8) |
                (rtp_pkg[7]) 
                ),
            payload:rtp_pkg.subarray(RTP_HEADER_LEN),
        };
    },
    on_rtp:function(pkg){    
        var rtp = this.parse_rtp(pkg);
        if(rtp.pt != PT.AUDIO_G711A){
            throw new Error('only support g711');
        }
        // print('on_rtp:'+JSON.stringify(rtp))
        return rtp;
    },
};
try{

    var poc = new Poc();
    var udp_recever_thread = undefined;
    function connect(){
        poc.connect();
        udp_recever_thread = undefined
        print('connected')
    }
    function reconnect(reconnect_ms){
        try{
            udp_recever_thread = co(connect)
            resume(udp_recever_thread,reconnect_ms)
        }catch(e){
            print(e);
            if(reconnect_ms < 3000){
                reconnect_ms += 1000;
            }
            setTimeout(reconnect,reconnect_ms);
        }
    }
    var player = iot.os.go('/js/app/player.js',8*1024,2);

    var timeout = cfg.ping_timeout;
    var buf = Uint8Array.allocPlain(MTU);
    
    var pingtimer = undefined;
    var pongtimer = undefined;
    reconnect(0);

    function onPongTimeout(){
        print('pong timeout')
        setTimeout(function(){
            reconnect(0);
        },0);
    }
    function onPingTimeout(){
        poc.ping()
        pongtimer = setTimeout(onPongTimeout,cfg.pong_timeout)
        pingtimer = setTimeout(onPingTimeout,timeout);
    }
    pingtimer = setTimeout(onPingTimeout,timeout);

    poc.udp.onData = function(){
        if(udp_recever_thread){
            try{
                resume(udp_recever_thread)
                return;
            }catch(e){
                udp_recever_thread = undefined;
                print("resume err:"+e)
            }
        }
        var rsize = poc.recv(buf);
        if(rsize <= 0){
            print('rsize is 0');
            return ;
        }
        clearTimeout(pingtimer);
        var pkg = buf.subarray(0,rsize); 
        var cmd = pkg[0];
        switch(cmd){
            case CMD.PONG:{
                print('pong')
                if(pongtimer){
                    clearTimeout(pongtimer);
                    pongtimer = undefined;
                }
            }break;
            default:{
                if(cmd & RTP_MASK){
                    try{
                        var rtp = poc.on_rtp(pkg);
                        if(rtp.payload.length > 0 ){
                            var pcmbase64 = Duktape.enc('base64',rtp.payload);
                            player.send_msg(pcmbase64);
                        }
                    }catch(e){
                        print(e);
                    }
                }
            }            
        }
        pingtimer = setTimeout(onPingTimeout,timeout);
    }
    
}catch(e){
    print("poc e:"+e);
    setTimeout(function(){
        reconnect(0);
    },0);
}

print('poc exit');