function print(txt) {
}
function setTimeout(ms,callback) {    
}

function co(func){	
    return new Duktape.Thread(func);	
}	
var yield = Duktape.Thread.yield;	
var resume = Duktape.Thread.resume;

var iot = {
    stringToUintArray:function(str){
        var length = str.length;
        var out = Uint8Array.allocPlain(length);
        for(var i = 0;i<length;i++){
            out[i] = str.charCodeAt(i);
        }
        return out;
    },
    network:{
        wait_network_ready:function(){},
    },
    tts:{
        say:function(text){},
    },
    socket:{
        Udp:function(){           
        },
        Test:function(){
        },
    },
    os:{
        MSG_GO_MSG:3,
        MSG_TO_CHILD_MSG:8,
        MSG_TO_PARENT_MSG:9,
        sleep:function(ms){
            //sleep millseconds
        },
        current_task:function(){
            return 0;
        },        
        send_msg:function(task,id,msg/**string */){
            return true;//true 成功,false 失败
        },
        wait_msg:function(task){
            return {
                id:0,
                msg:"",
            };
        },
        //创建线程
        //在新线程中执行js
        //js_file_name 全路径
        //stacksize 堆栈大小
        //pri 优先级 >= 0 越大越高
        //in js_file_name 可以访问 iot.os.parent 来得到创建它的线程对象
        //iot.os.parent:
        // {
        //     task:0,//线程id
        //     onMsg:function(msg){

        //     },
        //     send_msg:function(msg){

        //     },
        // }
        go:function(js_file_name,stacksize,pri){   
            return new this.TaskObject(0);
        },
        //被c调用
        TaskObject:function(task,msgId) {
            this.task = task;
            this.msgId = msgId;
        }
    },
    audio:{
        tone:function(v /**number*/){

        },
        setChannel:function(chan /**number */){

        },
        setSpeakerVol:function(vol /**number */){
            
        },
        record_pcm:function(seconds,onDataCallback){
            return 0;//0 ok,<= 0 errcode
        },
        record_stop:function(){

        },
        play_pcm_file:function(filename,cb /**function */){
            //cb: function(result/**number */)
            return -1;//0:ok,< 0 other errcode
        },
        play_stop:function(){

        },
        play_pcm_stream:function(buffer,cb){
            //cb: function(result/**number */)
            return -1;//0:ok,< 0 other errcode
        },
        record_pcm_file:function(filename,millseconds,cb){
            return -1;//0:ok,< 0 other errcode
        }
    },
    misc:{
        getImei:function(){
            //return IMEI string
            return "";
        }
    },
    fs:{
        FROM_BEGIN:0,
        FROM_CUR:1,
        FROM_END:2,
        FileInfo:function(){
            this.isdir = false;
            this.name = "";
            this.size = 0;
            this.createTime = new Date();
            this.modifyTime = new Date();
            this.accessTime = new Date();
        },
        mkdir:function(dirname){
        },
        ls:function(dirname){
            return [new iot.fs.FileInfo()];//返回dirname下的文件和列表
        },
        File:function(name){
            this.name = name;//文件路径
            this.fd = 0;
        },     
    },
    uart:{
        UART1:0,
        UART2:1,
        UART3:2,
        USB_UART:3,
        Uart:function(port,bitrate){
            this.port = 0;
            this.bitrate = 115200;
        },        
    },
    gpio:{
        GPIO_MODE_INPUT:1,//输入
        GPIO_MODE_OUTPUT:2,//输出
        GPIO_MODE_INPUT_INT:3,//中断输入
        PULL_UP:1,//上拉
        PULL_DOWN:0,//下拉
        GPIO_NO_INT:0,//无中断
        GPIO_INT_H_LEVEL:1,//高电平触发
        GPIO_INT_L_LEVEL:2,//低电平触发
        GPIO_INT_BOTH_EDGE:3,//双沿触发
        GPIO_INT_FALLING_EDGE:4,//上升沿触发
        GPIO_INT_RAISING_EDGE:5,//下降沿触发
        /**
         * 设置gpio管脚
         * @param {Number} pin
         * @param {Number} mode 
         * @param {Boolean} pullUpOrDown 
         * @param {Number} intType
         * @param {Number} debounce 去抖时间毫秒
         * @param {Function} int_cb function(pin,val)
         */
        setup:function(pin,mode,pullUpOrDown,intType,debounce,int_cb){
        },
        close:function (pin) {
        },
        output:function(pin,val){
            //val 0,1
        },
        /**
         * 读电平
         * @param {number} pin 
         */
        input:function(pin){
            return 0;
        }
    },
    keypad:{
        GPIOKEY_IRQ:0,/*普通模式*/
        GPIOKEY_ENCRYPT:1,/*加密模式*/
        /**
         * 配置矩阵键盘
         * @param {Number} rowMark ,行掩码,1有效
         * @param {Number} colMak 列掩码,1有效
         * @param {Function} cb function(row,col,press)
         */
        setup_m_keys:function(inMark,outMak,cb){

        },
        /**
         * 设置gpio键盘
         * @param {Number} rowMark 
         * @param {Number} colMark 
         * @param {Mode} mode
         * @param {Function} cb function(row,col,press)
         */
        setup_gpio_keys:function(rowMark,colMark,mode,cb){

        }
    }
};
iot.os.TaskObject.prototype = {//object
    send_msg:function(msg){
        iot.os.send_msg(this.task,this.msgId,msg);
        return true;
    },
    onMsg:function(msg){
    },
}

iot.uart.Uart.prototype = {
    close:function(){

    },
    read:function(data/**UintArray */){
        return 0;//read size
    },
    write:function(buf/**UintArray */){
        return 0;//write size
    },
};

iot.fs.File.prototype = {
    //return boolean 
    open:function (read,write) {
        return false;
    },
    close:function () {
        
    },
    //buf
    //@return readed size
    read:function(buf){
        return 0;
    },
    //return writed size
    write:function(data){
        return 0;
    },
    size:function(){
        return 0;//file size
    },
    seek:function(pos,from){
        return 0;//current file position
    },
    pos:function(){
        return 0;//current file position
    }
};

iot.socket.Udp.prototype = {
    recvfrom:function(buffer/*BufferArray*/){
        //return > 0  recv size, <= 0 error
        return 0;
    },
    sendto:function(data/*BufferArray*/){
        //return > 0 write size,<= 0 error
        return 0;
    },
    wait_recv:function(){
        return false;//true 有数据接收
    },
    //true:同步接收,false异步接收,onData被调用
    sync:function(isSync){

    }
}; 
iot.socket.Test.prototype = {
    log:function(){
        print('Test.log');
    }
};
var poc = {
    Poc:function(devId,imei){
        this.devId = "";
        this.imei = "012345678912345";
        this.sock = 0;
    }
};

poc.Poc.prototype= {
    connect:function(address,port){

    },
    disconnect:function(){

    },
    beginTalk:function(){

    },
    endTalk:function(){

    },
};

iot.codec = {
    G711A:function(){        
    }
};

iot.codec.G711A.prototype = {
    encode:function(pcm/** Uint8Array*/){
        return new Uint8Array(0);//g711a data
    },
    decode:function(g711a/** */){
        return new Uint8Array(0);//pcm data
    }
}

iot.pmd = {
    StartMode:{
        DEFAULT:0,//由系统决定
        ON:1,//强制开启
        OFF:2,//强制不开启
    },
    /**开机 
    *@param	{StartMode} simStartUpMode:		开启SIM卡方式
    *@param	{StartMode}	nwStartupMode:		开启协议栈方式
    *@return true 成功
    */
    powerOn:function(simStartUpMode,nwStartupMode){

    },
    /**
     * 关机
     */
    powerOff:function(){

    },
    enterDeepSleep:function(enter /**boolean */){

    },
    getBatteryInfo:function(){
        return {
            chargerStatus:false,/**boolean 充电器状态 */
            chargeState:0,/** 充电状态 */
            battStatus:false,/**boolean 充电状态*/
            battVolt:0,/**number 电池电压 */
            battLevel:0,/**int 电压等级 */
        }
    }
};