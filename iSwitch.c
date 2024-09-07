#include "iSwitch.h"
#include <string.h>

#define DebugPrintf(str, ...) 
const uint8_t mode_branch[3] = {iSW_SCAN_MODE0_0, iSW_SCAN_MODE1_0, iSW_SCAN_MODE2_0};
/*-----------------------------------------------------------------------
|                               Function                                |
-----------------------------------------------------------------------*/


#ifdef  USE_iSW_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void iSW_assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


static uint8_t iSWx_Do_Mode0(iSW_t* pSW)
{
    DebugPrintf("[iSW]Single Trigger,time = %d\n\r", pSW->status_time);
    pSW->scan_status = iSW_SCAN_IDLE;
    if(pSW->t.u.M0.trigger_way == 0)
        return iSW_EVENT_SET(pSW, iSW_EVENT_CLICK);
    pSW->t.u.M0.shadow_status = iSW_EVENT_CLICK;
    return 0;
}

#if (iSW_MODE1_ENABLE == 1)
static uint8_t iSWx_Do_Mode1(iSW_t* pSW)
{
    if(pSW->scan_status == iSW_SCAN_MODE1_0)//Mode1_0
    {
        pSW->scan_status = iSW_SCAN_MODE1_1;
        DebugPrintf("[iSW]Repeat Click Trigger,time = %d\n\r", pSW->status_time);
        iSW_EVENT_SET(pSW, iSW_EVENT_CLICK);
    }
    else if(pSW->scan_status == iSW_SCAN_MODE1_1)//Mode1_1
    {
        if( (pSW->status_time) > (pSW->t.u.M1.repeat_start_time) )
        {
            pSW->scan_status = iSW_SCAN_MODE1_2;
        }
    }
    else if(pSW->scan_status == iSW_SCAN_MODE1_2)//Mode1_2
    {
        if( ((pSW->t.u.M1.repeat_cnt) < (pSW->t.u.M1.max_trigger_cnt)) ||
            (pSW->t.u.M1.max_trigger_cnt == 0) )
        {
            if( (pSW->status_time - pSW->status_time_cnt) >= 
                pSW->t.u.M1.repeat_interval_time )
            {
                pSW->t.u.M1.repeat_cnt++;
                DebugPrintf("[iSW]Repeat Trigger,time = %d,%dth\n\r",
                            pSW->status_time, pSW->t.u.M1.repeat_cnt);
                pSW->status_time_cnt = pSW->status_time;
                iSW_EVENT_SET(pSW, iSW_EVENT_REPEAT);
            }
        }
        else
        {
            pSW->scan_status = iSW_SCAN_IDLE;
        }
    }
    return 0;
}
#endif

#if (iSW_MODE2_ENABLE == 1)
static uint8_t iSWx_Do_Mode2(iSW_t* pSW)
{
    if(pSW->scan_status == iSW_SCAN_MODE2_0)//Mode2_0
    {
        if(pSW->status_time >= pSW->t.u.M2.short_time)
        {
            if(pSW->t.u.M2.long_time != 0)
                pSW->scan_status = iSW_SCAN_MODE2_1;
            else
                pSW->scan_status = iSW_SCAN_IDLE;
            
            if(pSW->t.u.M2.trigger_way == 0)
                iSW_EVENT_SET(pSW, iSW_EVENT_SHORT);
            else
                pSW->t.u.M2.shadow_status = iSW_EVENT_SHORT;
            DebugPrintf("[iSW]Short Press Tirgger,time=%d,events=%d\n\r",
                        pSW->status_time, pSW->events);
        }
    }
    else if(pSW->scan_status == iSW_SCAN_MODE2_1)//Mode2_1
    {
        if(pSW->status_time >= pSW->t.u.M2.long_time)
        {
            // only trigger once when long press and trigger_way is 1
            pSW->t.u.M2.shadow_status = 0;
            iSW_EVENT_SET(pSW, iSW_EVENT_LONG);
            pSW->scan_status = iSW_SCAN_IDLE;
            DebugPrintf("iSW]Long Press Tirgger,time=%d,events=%d\n\r",
                        pSW->status_time, pSW->events);
        }
    }
    return 0;
}
#endif

static uint8_t iSWx_StateMachine(iSW_t* pSW, uint8_t state, uint16_t millisecond)
{
    if (pSW->status_time < 0xFFFF)
    {
        pSW->status_time += millisecond;
    }
    switch (pSW->scan_status)
    {
    case iSW_SCAN_INIT:
        if(state == pSW->trigger)
        {
            //SW debouncing
            pSW->scan_status = iSW_SCAN_DEBOUNCE;
            //The next scan time is the debounce time
            pSW->scan_time_cnt = pSW->debouncing_time;
            iSW_EVENT_SET(pSW, iSW_EVENT_PRESS);
        }
        break;
    case iSW_SCAN_DEBOUNCE:
        //debouncing......
        pSW->scan_time_cnt--;
        if (pSW->scan_time_cnt == 0)
        {
            pSW->scan_status = iSW_SCAN_TRIGGER;
        }
        break;
    case iSW_SCAN_TRIGGER:

        #if (iSW_DOUBLE_CLICK_ENABLE == 1)
        {
            // Check if the release time triggers a double-click here
            if(	pSW->double_min_time != 0 &&
                pSW->status_time < pSW->double_max_time &&
                pSW->status_time > pSW->double_min_time )
            {
                DebugPrintf("[iSW]double-click tirgger, time = %d\n\r",pSW->status_time);
                iSW_EVENT_SET(pSW, iSW_EVENT_DOUBLE_CLICK);
            }
        }
        #endif
        // Start recording the pressing time here
        pSW->status_time = 0;
        pSW->scan_status = mode_branch[pSW->t.mode];
        break;
    case iSW_SCAN_MODE0_0:
        iSWx_Do_Mode0(pSW);
        break;
    #if (iSW_MODE1_ENABLE == 1)
    case iSW_SCAN_MODE1_0:
    case iSW_SCAN_MODE1_1:
    case iSW_SCAN_MODE1_2:
        iSWx_Do_Mode1(pSW);
        break;
    #endif
    #if (iSW_MODE2_ENABLE == 1)
    case iSW_SCAN_MODE2_0:
    case iSW_SCAN_MODE2_1:
        iSWx_Do_Mode2(pSW);
        break;
    #endif
    case iSW_SCAN_IDLE:
        
        break;
    default:
        // error
        break;
    }
    if (state != pSW->trigger && pSW->scan_status != iSW_SCAN_INIT)
    {
        switch (pSW->t.mode)
        {
        case iSW_MODE_SINGLE:
            pSW->events |= pSW->t.u.M0.shadow_status;
            pSW->t.u.M0.shadow_status = 0;
            break;
        case iSW_MODE_REPEAT:
            #if (iSW_MODE1_ENABLE == 1)
            {
                pSW->t.u.M1.repeat_cnt = 0;
            }
            #endif
            break;
        case ISW_MODE_PRESS:
            #if (iSW_MODE2_ENABLE == 1)
            {
                // only trigger once,long preess or short press
                pSW->events |= pSW->t.u.M2.shadow_status & iSW_EVENT_SHORT;
                pSW->t.u.M2.shadow_status = 0;
            }
            #endif
            break;
        default:
            // error
            break;
        }
        //Clear MiddleFlag and Values
        iSW_EVENT_SET(pSW, iSW_EVENT_RELEASE);
        pSW->scan_status = iSW_SCAN_INIT;
        pSW->status_time_cnt = 0;
        //start release time count
        pSW->status_time = 1;
        return pSW->events;
    }
    return 0;
}

void iSW_Init(iSW_t *pSW, uint16_t length, uint8_t trigger, uint8_t debouncing_time)
{
    int i = 0;
    iSW_assert_param(pSW != NULL);
    for(i = 0; i < length; i++)
    {
        pSW[i].trigger = trigger;
        pSW[i].debouncing_time = debouncing_time;
        pSW[i].scan_status = iSW_SCAN_INIT;
        pSW[i].status_time_cnt = 0;
        pSW[i].status_time = 0xFFFF;
        pSW[i].t.mode = iSW_MODE_SINGLE;
        #if (iSW_DOUBLE_CLICK_ENABLE == 1)
        pSW[i].double_min_time = 0;
        #endif
    }
}

/**
  * @brief  Clear All iSW Events
  * @param  pSW :iSwitch Struct handle Array
            size:Array Length
  * @note 	that this function may cause access to be out of bounds
  * @retval triggered keys (32 bit)
  */
void iSW_Clear(iSW_t *pSW, uint32_t length)
{
    iSW_assert_param(pSW != NULL);
    while(length--)
    {
        (*pSW).events = 0;
        pSW++;
    }
    return;
}

/**
  * @brief  Scan key list
  * @param  pSW    :iSwitch Struct handle
  * @param  status :Key input status
  * @param  length :Key input length
  * @param  millisecond :Clock cycle [1ms~20ms]
  * @retval [0]Not trigger,[1]Trigger
  */
uint32_t iSW_Scan(iSW_t *pSW, const uint8_t *inputs, uint32_t length, uint16_t millisecond)
{
    int i;
    uint32_t triggerNum = 0;
    iSW_assert_param(pSW != NULL && inputs != NULL);
    for (i = 0; i < length; i++)
    {
        iSWx_StateMachine(&pSW[i], inputs[i], millisecond);
        if (pSW[i].events > 0)
        {
            triggerNum++;
        }
    }
    return triggerNum;
}

/**
  * @brief  Create a iSW_Mode_t and init 
  * @param  pSW :iSwitch handle
  * @param  triggerWay :trigger way [0:press trigger][1:release trigger]
  * @retval iSW_Mode_t handle
  */
void iSW_Set_Mode0(iSW_t *pSW, uint8_t triggerWay)
{
    iSW_Mode_Cfg_t imode;
    iSW_assert_param(pSW != NULL && way < 2);
    imode.mode = iSW_MODE_SINGLE;
    imode.u.M0.trigger_way = triggerWay;
    imode.u.M0.shadow_status = 0;
    iSW_Set_Mode(pSW, &imode);
    return;
}

#if (iSW_MODE1_ENABLE == 1)
/**
  * @brief  Create a iSW_Mode_t and init
  * @param  pSW            :iSwitch handle
  * @param  repeatDelay    :Delay time of repeat triggering
  * @param  repeatInterval :Time interval of continuous triggering
  * @param  max_cnt        :Maximum trigger times [0:unlimited]
  * @retval None
  */
void iSW_Set_Mode1(iSW_t *pSW,
                   uint16_t repeatDelay,
                   uint16_t repeatInterval,
                   uint16_t max_cnt)
{
    iSW_Mode_Cfg_t imode;
    iSW_assert_param(pSW != NULL);
    
        imode.mode = iSW_MODE_REPEAT;
        imode.u.M1.repeat_start_time = repeatDelay;
        imode.u.M1.max_trigger_cnt = max_cnt;
        imode.u.M1.repeat_interval_time = repeatInterval;
        imode.u.M1.repeat_cnt = 0;
        iSW_Set_Mode(pSW,&imode);
    
    return;
}
#endif

#if (iSW_MODE2_ENABLE == 1)
/**
  * @brief  Create a iSW_Mode_t and init
  * @param  pSW :iSwitch handle
  * @param  long_time :Long press time 
  * @param  short_time:short press time
  * @param  triggerWay:trigger way[0:immediately trigger][1:release trigger]
  * @retval None
  */
void iSW_Set_Mode2(iSW_t *pSW,
                   uint16_t long_time,
                   uint16_t short_time,
                   uint8_t triggerWay)
{
    iSW_Mode_Cfg_t imode;
    iSW_assert_param(pSW != NULL && triggerWay < 2);
    imode.mode = ISW_MODE_PRESS;
    imode.u.M2.long_time = long_time;
    imode.u.M2.short_time = short_time;
    imode.u.M2.trigger_way = triggerWay;
    imode.u.M2.shadow_status = 0;
    iSW_Set_Mode(pSW, &imode);
    return;
}
#endif

void iSW_Set_Mode(iSW_t *pSW, iSW_Mode_Cfg_t* Mode)
{
    iSW_assert_param(pSW != NULL && Mode != NULL);
    if(Mode->mode < 3)
    {
        memcpy(&(pSW->t),Mode,sizeof(pSW->t));
    }
    return;
}

#if (iSW_DOUBLE_CLICK_ENABLE == 1)
/**
  * @brief  Set Double Click
  * @param  pSW :iSwitch handle
  * @param  min :Minimum waiting time for double-click trigger
  * @param	max :Maximum waiting time for double-click trigger
  * @retval None
  */
void iSW_Set_Double_Click(iSW_t *pSW, uint8_t min, uint8_t max)
{
    iSW_assert_param(pSW != NULL);
    pSW->double_min_time = min;
    pSW->double_max_time = max;
}
#endif

/**
  * @brief  Get iSW Mode
  * @param  pSW :iSwitch handle
  * @retval Current mode
  */
uint8_t iSW_Get_Mode(iSW_t *pSW)
{
    iSW_assert_param(pSW != NULL);
    return pSW->t.mode;
}

/**
  * @brief  Get iSW Status
  * @param  pSW :iSwitch handle
  * @retval iSwitch_STATUS
  */
uint8_t iSW_Get_Events(iSW_t *pSW, uint8_t eventMask)
{
    uint8_t ret;
    iSW_assert_param(pSW != NULL);
    ret = pSW->events & eventMask;
    /* Auto Clear Trigger Flag */
    iSW_EVENT_CLEAR(pSW, eventMask);
    return ret;
}
