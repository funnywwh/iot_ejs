#ifndef __GPIO_H__
#define __GPIO_H__


typedef enum
{
  GPIO_INVALID_MODE = 0,
  GPIO_INPUT = 1, //as gpio or gpo
  GPIO_OUTPUT = 2,
  GPIO_INPUT_INT = 3,
  GPIO_MODE_UNKNOWN = 4,
}T_GPIO_MODE;

typedef void (*GPIO_INT_CALLBACK)(int pin,int val,void* udata);
int platform_gpio_setup(int pin,int mode,int pullUpOrDown,int intType,int debounce/*millseconds*/,GPIO_INT_CALLBACK cb,void* udata);
int platform_gpio_close(int pin);
int platform_gpio_output(int pin,int val);
//读gpio管脚
//返回 0：低电平 1:高电平
int platform_gpio_input(int pin);
#endif //