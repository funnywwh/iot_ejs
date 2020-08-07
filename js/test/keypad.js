iot.keypad.setup_m_keys(0xff,0xff,function(row,col,press){
    print('keypad m callback. row:'+row+' col:'+col+" press:"+press);
});
iot.keypad.setup_gpio_keys(0x1,0x1,iot.keypad.GPIOKEY_IRQ,function(row,col,press){
    print('keypad gpio callback. row:'+row+' col:'+col+" press:"+press);
});
