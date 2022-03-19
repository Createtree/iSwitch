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

	unsigned char shadow_status;	//<auto>Ӱ��״̬λ�����ֺ�ת�Ƶ�status
	unsigned char trigger_way;		//<Mode0>������ʽ[0:���´���][1:�ɿ�����]
}iSwitch_Mode0;
		
typedef struct iSwitch_Mode1_t{
	
	unsigned int trigger_cnt;		//<auto>���������Ĵ���
	unsigned int scan_time;		//<Mode1>�������������ļ��ʱ��[1,2^32-1](ms)
	unsigned int max_trigger_cnt;	//<Mode1>���������������[1,2^32-1]/[0]��ʾ���޴���������							  
	unsigned int delay_time;		//<Mode1>�������º�������������Ӧʱ�佨��Ĭ��300ms
}iSwitch_Mode1;


typedef struct iSwitch_Mode2_t{
	
	unsigned char shadow_status;	//<auto>Ӱ��״̬λ�����ֺ�ת�Ƶ�status
	unsigned char trigger_way;		//<Mode2>������ʽ[0:���´���][1:�ɿ�����]
	unsigned int short_time;		//<Mode2>�����̰��Ĵ���ʱ��
	unsigned int long_time;		//<Mode2>�������Ĵ���ʱ��
}iSwitch_Mode2;

typedef union iSwitch_Mode_u{

	iSwitch_Mode0 M0;
	iSwitch_Mode1 M1;
	iSwitch_Mode2 M2;
}iSW_Mode_u;	

typedef struct iSwitch_Mode_t{
	unsigned char mode; 		/**--------<must>����ģʽ-------------
								@Mode0:������ģʽ
								@Mode1:������������ģʽ(ͨ��max_cnt����)
								@Mode2:�̰�������
								------------------------------------**/	
	iSW_Mode_u u;
}iSW_Mode_t;

typedef struct Switch_Handle_TypeDef//�����������
{
	const unsigned char id;			//<must>ʵ��ID[0,31]
	unsigned char trigger;		//<must>���������Ĵ�����ƽ[0]/[1]
	unsigned char shake_time;	//<must>����ʱ��(ms)


	unsigned char status;				//<auto>����״̬Bit[1:����]/[0:δ����],�ο�enum iSwitch_STATUS
	unsigned int scan_time_cnt;		//<auto>����ɨ��ʣ��ĵȴ�ʱ��[1,2^32-1](ms)
	unsigned char scan_status;			//<auto>�������º��ɨ��״̬[0:ȥ��̬][99:��̬][100:����̬]

	unsigned int status_time;			//<auto>�ͷŵ�ʱ��/���µ�ʱ��
	unsigned int status_time_cnt;		//<auto>��¼���µ�ʱ��
	//<optional>
	unsigned char double_max_time;		//<optional>˫�������Ӧ��ֵ[0,255](ms)
	unsigned char double_min_time;		//<optional>˫����С��Ӧ��ֵ(0:���ù���,255]
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


#define iSW_LONG 20		//�������������32��
#define SHAKE_TIME 10	//Ĭ������ʱ��(ms)
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
