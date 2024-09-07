# iSwitch 

## 简介

>iSwitch 是一个针对按键的驱动，它能对按键进行软件去抖，支持多个按键同时按下无冲突，同时拓展了按键的功能，如长按、短按、连续触发、松手触发、组合键等。

## 使用要求

- 1~20ms的定时调用

## 使用方法

> 参考 `bsp_key.h`

```C
#include ...
// 调试打印
#define debugPrint(str, ...) UARTx_Printf(&huart1, str, ##__VA_ARGS__)
// 按键输入电平读取
#define Bsp_Key_ReadPin(Port, Pin) HAL_GPIO_ReadPin(Port, Pin)
/* [1] 创建按键对象 */
iSW_t hisw1[BSP_KEY_NUM] = {0};

/* [2] 实现按键电平读取接口 */
void bsp_key_input_read(uint8_t *results, uint16_t num)
{
    if (results == NULL || num == 0)
        return ;
    results[0] = Bsp_Key_ReadPin(SW1_GPIO_Port, SW1_Pin);
    results[1] = Bsp_Key_ReadPin(SW2_GPIO_Port, SW2_Pin);
    results[2] = Bsp_Key_ReadPin(SW3_GPIO_Port, SW3_Pin);
    results[3] = Bsp_Key_ReadPin(SW4_GPIO_Port, SW4_Pin);
}

/* ---[3] 初始按键并配置功能--- */
void bsp_key_init(void)
{
    // 初始化按键 设置触发为低电平，消抖时间为20ms
    iSW_Init(hisw1, BSP_KEY_NUM, iSW_TRIGGER_LEVEL_LOW, 20);
    // 设置按键0为模式0，触发方式为按下触发，双击时间为40ms~200ms
    iSW_Set_Mode0(&hisw1[0], iSW_TRIGGER_WAY_PRESS);
    iSW_Set_Double_Click(&hisw1[0], 40, 200);
    // 设置按键1为模式1，连发延时为1s，连发间隔为100ms，次数为无限次
    iSW_Set_Mode1(&hisw1[1], 1000, 100, ISW_TRIGGER_CNT_INF);
    // 设置按键2为模式1，连发延时为1s，连发间隔为100ms，次数为无限次
    // 双击时间为40ms~100ms
    iSW_Set_Mode1(&hisw1[2], 1000, 100, ISW_TRIGGER_CNT_INF);
    iSW_Set_Double_Click(&hisw1[2], 40, 100);
    // 设置按键3为模式2，长按时间为2s，短按时间为20ms 触发方式为松开触发
    iSW_Set_Mode2(&hisw1[3], 2000, 20, iSW_TRIGGER_WAY_RELEASE);
}

void TaskInput(void const * argument)
{
    uint8_t key_input[BSP_KEY_NUM];
    bsp_key_init(); // 初始化按键设置
    while(1)
    {
        /* [4] 在任务或定时器(1~20ms)中读取按键输入数据，并使用iSW_Scan来扫描输入数据 */
        bsp_key_input_read(key_input, BSP_KEY_NUM);
        if (iSW_Scan(hisw1, key_input, BSP_KEY_NUM, 10)) // 10ms扫描一次
        {
            /* [5] 按键事件处理 */
            if (hisw1[0].events)
            {
                if (hisw1[0].events & iSW_EVENT_CLICK)
                {
                    debugPrint("KEY0 click\n\r");
                }
            }
            if (hisw1[1].events)
            {
                if (hisw1[1].events & iSW_EVENT_CLICK)
                {
                    debugPrint("KEY1 click\n\r");
                }
                if (hisw1[1].events & iSW_EVENT_REPEAT)
                {
                    debugPrint("KEY1 repeat:%d\n\r", iSW_Get_RepeatCnt(&hisw1[1]));
                }
            }
            if (hisw1[2].events)
            {
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
                if (hisw1[3].events & iSW_EVENT_LONG)
                {
                    debugPrint("KEY3 long press\n\r");
                }
                if (hisw1[3].events & iSW_EVENT_SHORT)
                {
                    debugPrint("KEY3 short press\n\r");
                }
            }
        }
        // 清空按键事件
        iSW_Clear(hisw1, BSP_KEY_NUM);
        // 延时10ms
        osDelay(10);
    }
}

```

