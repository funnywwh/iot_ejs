var cfg = require('app/config');

print('app runing');

iot.audio.setChannel(2);
iot.audio.setSpeakerVol(100);
iot.gpio.setup(9,iot.gpio.GPIO_MODE_OUTPUT)
iot.gpio.output(9,1)
iot.tts.say("正在联网")

iot.network.wait_network_ready();
iot.os.go('/js/app/poc.js');

iot.tts.say("已联网")
