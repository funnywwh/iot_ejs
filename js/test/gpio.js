iot.gpio.setup(0,iot.gpio.GPIO_MODE_OUTPUT);
iot.gpio.output(0,1);
iot.gpio.close(0);

iot.gpio.setup(2,iot.gpio.GPIO_MODE_INPUT,true);

function gpio3_cb(pin,val){
    print('gpio int pin:'+pin+" val:"+val);
    iot.gpio.close(3);
    if(val){
        iot.gpio.setup(3,iot.gpio.GPIO_MODE_INPUT_INT,true,iot.gpio.GPIO_INT_L_LEVEL,100,gpio3_cb);
    }else{
        iot.gpio.setup(3,iot.gpio.GPIO_MODE_INPUT_INT,false,iot.gpio.GPIO_INT_H_LEVEL,100,gpio3_cb)
    }
    print('gpio2 val:'+iot.gpio.input(2));
}
iot.gpio.setup(3,iot.gpio.GPIO_MODE_INPUT_INT,true,iot.gpio.GPIO_INT_L_LEVEL,100,gpio3_cb);
