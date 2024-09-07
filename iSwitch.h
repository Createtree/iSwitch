/*-----------------------------------------------------------------------
|                            FILE DESCRIPTION                           |
-----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
  - File name     : zh_iSwitch.h
  - Author        : liuzhihua
  - Update date   : 2024.9.7
  -	File Function : independent siwtch drivers
-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
|                               UPDATE NOTE                             |
-----------------------------------------------------------------------*/
/**
  * Update note:
  * ------------   ---------------   ----------------------------------
  *     Date            Author                       Note
  * ------------   ---------------   ----------------------------------
  *   2022.1.13       liuzhihua                   Create file
  *   2022.1.26       liuzhihua		The clipping macro has been updated to save memory
  *	  2022.3.1        liuzhihua           Optimize program structure
  *   2024.7.13       liuzhihua                 Refactoring code
  *   2024.9.7        liuzhihua     Modify the triggering way of long press
***/

#ifndef __ISWITCH_H_
#define __ISWITCH_H_

/*-----------------------------------------------------------------------
|                               INCLUDES                                |
-----------------------------------------------------------------------*/

#include <stdint.h>

#ifdef  __cplusplus
    extern "C" {
#endif

#define iSW_MODE1_ENABLE        (1)
#define iSW_MODE2_ENABLE        (1)
#define iSW_DOUBLE_CLICK_ENABLE (1)
/*-----------------------------------------------------------------------
|                                DEFINES                                |
-----------------------------------------------------------------------*/

/* --------------------- Exported macro ---------------------*/
#ifdef  USE_iSW_FULL_ASSERT
#define iSW_assert_param(expr) ((expr) ? (void)0U : iSW_assert_failed((uint8_t *)__FILE__, __LINE__))
void iSW_assert_failed(uint8_t* file, uint32_t line);
#else
#define iSW_assert_param(expr) ((void)0U)
#endif

struct iSwitch_Mode0_TypeDef {

    uint8_t shadow_status;     //<private>影子状态位，松手后转移到status
    uint8_t trigger_way;       //<Mode0>触发方式[0:按下触发][1:松开触发]
};

struct iSwitch_Mode1_TypeDef {

    uint16_t repeat_cnt;             //<private>按键触发的次数
    uint16_t repeat_interval_time;   //<Mode1>按键连续触发的间隔时间[1,2^16-1](ms)
    uint16_t max_trigger_cnt;        //<Mode1>按键最大连按次数[1,2^16-1]/[0]表示无限次连续触发
    uint16_t repeat_start_time;      //<Mode1>按键按下后连续触发的响应时间建议默认300ms
};

struct iSwitch_Mode2_TypeDef {

    uint8_t shadow_status;    //<private>影子状态位，松手后转移到status
    uint8_t trigger_way;      //<Mode2>触发方式[0:按下触发][1:松开触发]
    uint16_t short_time;      //<Mode2>按键短按的触发时间
    uint16_t long_time;       //<Mode2>按长按的触发时间
};

typedef struct iSwitch_Mode_Cfg_TypeDef {
    /**--------<userConfig>按键模式-------------
        @Mode0:单触发模式
        @Mode1:连续触发模式(通过max_cnt参数)
        @Mode2:短按、长按
    ------------------------------------**/
    uint8_t mode;
    union {
        struct iSwitch_Mode0_TypeDef M0;
        #if (iSW_MODE1_ENABLE == 1)
            struct iSwitch_Mode1_TypeDef M1;
        #endif
        #if (iSW_MODE2_ENABLE == 1)
            struct iSwitch_Mode2_TypeDef M2;
        #endif
    } u;
}iSW_Mode_Cfg_t;

//按键句柄定义
typedef struct iSwitch_handle_Typedef {
    /*--------------------- <userConfig> ---------------------*/
    uint8_t trigger;            // 按键触发的触发电平[0]/[1]
    uint8_t debouncing_time;    // 消抖时间[0,255](tick)
    #if (iSW_DOUBLE_CLICK_ENABLE == 1)
        uint8_t double_max_time;// 双击最大响应阈值[0,255](tick)
        uint8_t double_min_time;// 双击最小响应阈值[0:(禁用功能),255](tick)
    #endif
    iSW_Mode_Cfg_t t;           // 模式配置
    /*--------------------- <userReadOnly> ---------------------*/
    uint8_t events;             // 按键状态(参考iSW_Event定义)
    uint8_t scan_status;        // 按键按下后的扫描状态(参考iSW_ScanStatus_TypeDef定义)
    uint16_t scan_time_cnt;     // 按键扫描剩余的等待时间[1,2^16-1](tick)
    uint16_t status_time;       // 释放的时间/按下的时间(tick)
    uint16_t status_time_cnt;   // 记录按下的时间(tick)
}iSW_t;

typedef enum iSwitch_Mode_Typedef{
    iSW_MODE_SINGLE = 0,
    iSW_MODE_REPEAT = 1,
    ISW_MODE_PRESS  = 2
}iSW_Mode_t;

enum iSW_ScanStatus_TypeDef{
    iSW_SCAN_INIT = 0,
    iSW_SCAN_DEBOUNCE,
    iSW_SCAN_TRIGGER,
    iSW_SCAN_MODE0_0,
    iSW_SCAN_MODE1_0,
    iSW_SCAN_MODE1_1,
    iSW_SCAN_MODE1_2,
    iSW_SCAN_MODE2_0,
    iSW_SCAN_MODE2_1,
    iSW_SCAN_IDLE
};

typedef enum iSwitch_Event_TypeDef{

    iSW_EVENT_PRESS         = 1<<0,  // 按下
    iSW_EVENT_RELEASE       = 1<<1,  // 释放
    iSW_EVENT_CLICK         = 1<<2,  // 单击
    iSW_EVENT_REPEAT        = 1<<3,  // 连续触发
    iSW_EVENT_SHORT         = 1<<4,  // 短按
    iSW_EVENT_LONG          = 1<<5,  // 长按
    iSW_EVENT_DOUBLE_CLICK  = 1<<6,  // 双击
    iSW_EVENT_ALL          = 0xFF    // 全部事件
}iSW_Event_t;

#define iSW_TRIGGER_LEVEL_LOW  0
#define iSW_TRIGGER_LEVEL_HIGH 1
#define iSW_TRIGGER_WAY_PRESS  0
#define iSW_TRIGGER_WAY_RELEASE 1
#define ISW_TRIGGER_CNT_INF    0
/*-----------------------------------------------------------------------
|                             API FUNCTION                              |
-----------------------------------------------------------------------*/
#define iSW_IS_PRESS(pSW)        ((pSW)->scan_status != iSW_SCAN_INIT)
#define iSW_EVENT_CLEAR(pSW, x)  ((pSW)->events &= ~(x))
#define iSW_EVENT_SET(pSW, x) 	 ((pSW)->events |= (x))
#define iSW_Set_Signle           iSW_Set_Mode0
#define iSW_Set_Repeat           iSW_Set_Mode1
#define iSW_Set_Press            iSW_Set_Mode2
void iSW_Init(iSW_t *pSW, uint16_t length, uint8_t trigger, uint8_t debouncing_time);
uint32_t iSW_Scan(iSW_t *pSW, const uint8_t *inputs, uint32_t length, uint16_t millisecond);
void iSW_Set_Mode(iSW_t *pSW, iSW_Mode_Cfg_t* Mode);
void iSW_Set_Mode0(iSW_t *pSW, uint8_t triggerWay);
#if (iSW_MODE1_ENABLE == 1)
#define iSW_Get_RepeatCnt(pSW)   ((pSW)->t.u.M1.repeat_cnt)
    void iSW_Set_Mode1(iSW_t *pSW,
                    uint16_t repeatDelay,
                    uint16_t repeatInterval,
                    uint16_t max_cnt);
#else
    #define iSW_Get_RepeatCnt(pSW)   0
    #define iSW_Set_Mode1( pSW, repeatDelay, repeatInterval, max_cnt)
#endif
#if (iSW_MODE2_ENABLE == 1)
    void iSW_Set_Mode2(iSW_t *pSW,
                    uint16_t long_time,
                    uint16_t short_time,
                    uint8_t triggerWay);
#else
    #define iSW_Set_Mode2(pSW, long_time, short_time, triggerWay)
#endif
#if (iSW_DOUBLE_CLICK_ENABLE == 1)
    void iSW_Set_Double_Click(iSW_t *pSW, uint8_t min, uint8_t max);
#else
    #define iSW_Set_Double_Click(pSW, min, max);
#endif
uint8_t iSW_Get_Mode(iSW_t *pSW);
uint8_t iSW_Get_Events(iSW_t *pSW, uint8_t eventMask);
void iSW_Clear(iSW_t *pSW, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
