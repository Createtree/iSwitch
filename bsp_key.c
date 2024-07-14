#include "bsp_key.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "bsp_uart.h"

// 调试打印
#define debugPrint(str, ...) UARTx_Printf(&huart1, str, ##__VA_ARGS__)
// 按键输入电平读取
#define Bsp_Key_ReadPin(Port, Pin) HAL_GPIO_ReadPin(Port, Pin)
/* [1] 创建按键对象 */
iSW_t hisw1[BSP_KEY_NUM] = {0};

/* [2] 实现按键电平读取接口 */
void bsp_key_input_raed(uint8_t *results, uint16_t num)
{
    if (results == NULL || num == 0)
        return ;
    results[0] = Bsp_Key_ReadPin(SW1_GPIO_Port, SW1_Pin);
    results[1] = Bsp_Key_ReadPin(SW2_GPIO_Port, SW2_Pin);
    results[2] = Bsp_Key_ReadPin(SW3_GPIO_Port, SW3_Pin);
    results[3] = Bsp_Key_ReadPin(SW4_GPIO_Port, SW4_Pin);
}

void TaskInput(void const * argument)
{
    uint8_t key_input[BSP_KEY_NUM];
    /*-------------------[3] 初始按键并配置功能-------------------*/
    // 初始化按键 设置触发为低电平，消抖时间为2tick(20ms)
    iSW_Init(hisw1, BSP_KEY_NUM, iSW_TRIGGER_LEVEL_LOW, 2);
    // 设置按键0为模式0，触发方式为按下触发，双击时间为4~10tick(40Ms~200ms)
    iSW_Set_Mode0(&hisw1[0], iSW_TRIGGER_WAY_PRESS);
    iSW_Set_Double_Click(&hisw1[0], 4, 20);
    // 设置按键1为模式1，连发延时为100tick(1s)，连发间隔为10tick(100ms)，次数为无限次
    iSW_Set_Mode1(&hisw1[1], 100, 10, 0);
    // 设置按键2为模式1，连发延时为100tick(1s)，连发间隔为10tick(100ms)，次数为无限次
    // 双击时间为4~10tick(40Ms~100ms)
    iSW_Set_Mode1(&hisw1[2], 100, 10, 0);
    iSW_Set_Double_Click(&hisw1[2], 4, 10);
    // 设置按键3为模式2，长按时间为200tick(2s)，短按时间为2tick(20ms) 触发方式为按下触发
    iSW_Set_Mode2(&hisw1[3], 200, 2, iSW_TRIGGER_WAY_PRESS);
    while(1)
    {
        /* [4] 在任务或定时器(1~20ms)中读取按键输入数据，并使用iSW_Scan来扫描输入数据 */
        bsp_key_input_raed(key_input, BSP_KEY_NUM);
        if (iSW_Scan(hisw1, key_input, BSP_KEY_NUM))
        {
            /* [5] 按键事件处理 */
            if (hisw1[0].events)
            {
                if (hisw1[0].events & iSW_EVENT_PRESS)
                {
                    debugPrint("KEY0 press start\n\r");
                }
                if (hisw1[0].events & iSW_EVENT_CLICK)
                {
                    debugPrint("KEY0 click\n\r");
                }
                if (hisw1[0].events & iSW_EVENT_DOUBLE_CLICK)
                {
                    debugPrint("KEY0 double click\n\r");
                }
                if (hisw1[0].events & iSW_EVENT_RELEASE)
                {
                    debugPrint("KEY0 release\n\r");
                }
            }
            if (hisw1[1].events)
            {
                if (hisw1[1].events & iSW_EVENT_PRESS)
                {
                    debugPrint("KEY1 press start\n\r");
                }
                if (hisw1[1].events & iSW_EVENT_CLICK)
                {
                    debugPrint("KEY1 click\n\r");
                }
                if (hisw1[1].events & iSW_EVENT_REPEAT)
                {
                    debugPrint("KEY1 repeat:%d\n\r", iSW_Get_RepeatCnt(&hisw1[1]));
                }
                if (hisw1[1].events & iSW_EVENT_RELEASE)
                {
                    debugPrint("KEY1 release\n\r");
                }
            }
            if (hisw1[2].events)
            {
                if (hisw1[2].events & iSW_EVENT_CLICK)
                {
                    debugPrint("KEY2 click\n\r");
                }
                if (hisw1[2].events & iSW_EVENT_REPEAT)
                {
                    debugPrint("KEY2 repeat:%d\n\r", iSW_Get_RepeatCnt(&hisw1[2]));
                }
                if (hisw1[2].events & iSW_EVENT_DOUBLE_CLICK)
                {
                    debugPrint("KEY2 double click\n\r");
                }
                // 组合键
                if (iSW_IS_PRESS(&hisw1[0]))
                {
                    debugPrint("KEY0 and KEY2 press\n\r");
                }
            }
            if (hisw1[3].events)
            {
                if (hisw1[3].events & iSW_EVENT_PRESS)
                {
                    debugPrint("KEY3 press start\n\r");
                }
                if (hisw1[3].events & iSW_EVENT_LONG)
                {
                    debugPrint("KEY3 long press\n\r");
                }
                if (hisw1[3].events & iSW_EVENT_SHORT)
                {
                    debugPrint("KEY3 short press\n\r");
                }
                if (hisw1[3].events & iSW_EVENT_RELEASE)
                {
                    debugPrint("KEY3 release\n\r");
                }
            }
        }
        // 清空按键事件
        iSW_Clear(hisw1, BSP_KEY_NUM);
        // 延时10ms.所以1个tick是10ms
        osDelay(10);
    }
}


