setTimeout(function(){
    print("getting batteryInfo")
    var batteryInfo = iot.pmd.getBatteryInfo();
    print(Duktape.enc('jx',batteryInfo));
    print("will poweroff")
    var ret = iot.pmd.powerOff()
    print("poweroff:"+ret)
},3000);