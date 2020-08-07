print("player running")
try{
    var MAX_PCM_FILE_SIZE = 8000*2*1;
    var pcm_file = undefined;
    var file_size = 0;
    var toplay = false;
    var onMsgTimeer = 0;
    var g711a = new iot.codec.G711A();
    iot.os.parent.onMsg = function(msg) {
        try{
            var g711abuf = Duktape.dec('base64',msg);
            var buf = g711a.decode(g711abuf);
            file_size += buf.length;
            if(!pcm_file){
                pcm_file = new iot.fs.File('/'+performance.now()+".pcm");
                pcm_file.open(false,true);
            }
            if(onMsgTimeer){
                clearTimeout(onMsgTimeer);
            }
            onMsgTimeer = setTimeout(function(){  
                try{
                    onMsgTimeer = 0;
                    var filename = pcm_file.name;
                    pcm_file.close();
                    pcm_file = undefined;
                    iot.audio.play_pcm_file(filename,function (result) {
                        iot.fs.delFile(filename);
                        toplay = true;
                    })
                }catch(e){
                    print(e)
                }           
            }, 500);
            pcm_file.write(buf);
        }catch(e){
            print(e);
        }
    };
}catch(e){
    print(e);
}
