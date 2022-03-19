/*-----------------------------------------------------------------------
|                            FILE DESCRIPTION                           |
-----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
  - File name     : zh_iSwitch.h
  - Author        : liuzhihua
  - Update date   : 2022.1.13                  
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
  *	  2022.3.1        liuzhihau           Optimize program structure
***/

#ifndef __ZH_ISWITCH_H_
#define __ZH_ISWITCH_H_

#ifdef  __cplusplus
    extern "C" {
#endif
/*-----------------------------------------------------------------------
|                               INCLUDES                                |
-----------------------------------------------------------------------*/
#include "string.h"


		
/*-----------------------------------------------------------------------
|                                DEFINES                                |
-----------------------------------------------------------------------*/
#ifndef NULL
#define NULL (void*)0
		
#endif
/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_iSW_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
#define iSW_assert_param(expr) ((expr) ? (void)0U : iSW_assert_failed((unsigned char *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void iSW_assert_failed(unsigned char* file, unsigned int line);
#else
#define iSW_assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

typedef struct iSwitch_Mode0_t{

	unsigned char shadow_status;	//<auto>影子状态位，松手后转移到status
	unsigned char trigger_way;		//<Mode0>触发方式[0:按下触发][1:松开触发]
}iSwitch_Mode0;
		
typedef struct iSwitch_Mode1_t{
	
	unsigned int trigger_cnt;		//<auto>按键触发的次数
	unsigned int scan_time;		//<Mode1>按键连续触发的间隔时间[1,2^32-1](ms)
	unsigned int max_trigger_cnt;	//<Mode1>按键最大连按次数[1,2^32-1]/[0]表示无限次连续触发							  
	unsigned int delay_time;		//<Mode1>按键按下后连续触发的响应时间建议默认300ms
}iSwitch_Mode1;


typedef struct iSwitch_Mode2_t{
	
	unsigned char shadow_status;	//<auto>影子状态位，松手后转移到status
	unsigned char trigger_way;		//<Mode2>触发方式[0:按下触发][1:松开触发]
	unsigned int short_time;		//<Mode2>按键短按的触发时间
	unsigned int long_time;		//<Mode2>按长按的触发时间
}iSwitch_Mode2;

typedef union iSwitch_Mode_u{

	iSwitch_Mode0 M0;
	iSwitch_Mode1 M1;
	iSwitch_Mode2 M2;
}iSW_Mode_u;	

typedef struct iSwitch_Mode_t{
	unsigned char mode; 		/**--------<must>按键模式-------------
								@Mode0:单触发模式
								@Mode1:连续触发控制模式(通过max_cnt参数)
								@Mode2:短按、长按
								------------------------------------**/	
	iSW_Mode_u u;
}iSW_Mode_t;

typedef struct Switch_Handle_TypeDef//按键句柄定义
{
	const unsigned char id;			//<must>实例ID[0,31]
	unsigned char trigger;		//<must>按键触发的触发电平[0]/[1]
	unsigned char shake_time;	//<must>消抖时间(ms)


	unsigned char status;				//<auto>按键状态Bit[1:触发]/[0:未触发],参考enum iSwitch_STATUS
	unsigned int scan_time_cnt;		//<auto>按键扫描剩余的等待时间[1,2^32-1](ms)
	unsigned char scan_status;			//<auto>按键按下后的扫描状态[0:去抖态][99:初态][100:空闲态]

	unsigned int status_time;			//<auto>释放的时间/按下的时间
	unsigned int status_time_cnt;		//<auto>记录按下的时间
	//<optional>
	unsigned char double_max_time;		//<optional>双击最大响应阈值[0,255](ms)
	unsigned char double_min_time;		//<optional>双击最小响应阈值(0:禁用功能,255]
	//<Mode config>
	iSW_Mode_t t;
	
}Switch_t;

typedef enum iSwitch_Mode_ENUM{
	iSW_Mode0=0,
	iSW_Mode1,
	ISW_Mode2
}iSW_Mode;

#define iSW(x) iSW##x
typedef enum iSwitch_ID_ENUM{
	iSW(0)=0,
	iSW(1),
	iSW(2),
	iSW(3),
	iSW(4),
	iSW(5),
	iSW(6),
	iSW(7),
	iSW(8),
	iSW(9),
	iSW(10),
	iSW(11),
	iSW(12),
	iSW(13),
	iSW(14),
	iSW(15)
}iSW_ID;

typedef enum iSwitch_STATUS{
	iSW_Mode0_Bit			= 0,
	iSW_Mode1_Bit			= 1,
	iSW_Mode2_Short_Bit		= 2,
	iSW_Mode2_Long_Bit		= 3,
	iSW_Double_Click_Bit	= 4,

}iSW_STATUS;


#define iSW_New_Mode() 


#define iSW_LONG 20		//按键数量，最大32个
#define SHAKE_TIME 10	//默认消抖时间(ms)
#define ISWITCH_DEBUG 0
/*-----------------------------------------------------------------------
|                             API FUNCTION                              |
-----------------------------------------------------------------------*/
unsigned int iSWx_Scan(unsigned char status,Switch_t* pSwitc);
unsigned int iSWx_Handler(unsigned int status32);

iSW_Mode_t iSW_To_Mode0(iSW_ID iSW_id,unsigned char way);
iSW_Mode_t iSW_To_Mode1(iSW_ID iSW_id,unsigned int delay,unsigned int scan_time,unsigned int max_cnt);
iSW_Mode_t iSW_To_Mode2(iSW_ID iSW_id,unsigned int long_time,unsigned int short_time,unsigned char way);
unsigned char iSW_Set_Mode(iSW_ID iSW_id,iSW_Mode_t* Mode);
unsigned char iSW_Get_Mode(iSW_ID iSW_id);

Switch_t* iSW_Get_Handle(iSW_ID iSW_id);
unsigned char iSW_Set_Double_Click(iSW_ID id,unsigned char max,unsigned char min);
unsigned char iSW_Get_Status(iSW_ID iSW_id,iSW_STATUS Bit);
unsigned char iSW_Get_All_Status(iSW_ID iSW_id);
void iSW_Clear(Switch_t *pSW, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif
