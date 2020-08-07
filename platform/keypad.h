#ifndef __KEYPAD_H__
#define __KEYPAD_H__
typedef enum E_KEYPAD_TYPE_TAG
{
    KEYPAD_TYPE_MATRIX,      /* 阵列键盘 */
    KEYPAD_TYPE_ADC,         /* ADC键盘 */
    KEYPAD_TYPE_GPIO,        /* GPIO键盘 */
    KEYPAD_TYPE_MAX
}E_KEYPAD_TYPE;
typedef enum E_GPIOKEY_MODE_TAG
{
    GPIOKEY_IRQ, /*普通模式*/
    GPIOKEY_ENCRYPT, /*加密模式*/
        
    GPIOKEY_MAX
}E_GPIOKEY_MODE;

typedef void (*KEYPAD_CALLBACK)(E_KEYPAD_TYPE type,int row,int col,int press);
int platform_setup_m_keys(int rowMark,int colMark,KEYPAD_CALLBACK cb);

int platform_setup_gpio_keys(int rowMark,int colMark,E_GPIOKEY_MODE mode,KEYPAD_CALLBACK cb);
#endif //__KEYPAD_H__