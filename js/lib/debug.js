//在独立的线程里执行，出来调试辅助功能
print("debug.js");
try{
    print('parent task:'+iot.os.parent.task);
    iot.os.parent.onMsg = function(msg){
        print('sub   :<='+msg);
        iot.os.sleep(1000);
        this.send_msg('ping');    
    }
    iot.os.parent.send_msg('ping');
}catch(e){
    print("debug e:"+e);
}
