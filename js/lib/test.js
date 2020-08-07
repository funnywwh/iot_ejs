var files = iot.fs.ls('/js/test');
for(var i in files){
    var fi = files[i];
    if(!fi.isdir && fi.name && fi.name.endsWith('.js')){
        print('testing '+fi.name);
        print('====================================');
        try{
            require('test/'+fi.name.substr(0,fi.name.length-3));
        }catch(e){
            print('test error:'+e);
        }
        print('====================================');
    }
}