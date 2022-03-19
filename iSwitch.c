#include "iSwitch.h"



#define STATUS_RESET(x) pSW->status &= ~(1<<x)
#define STATUS_SET(x) 	pSW->status |=  1<<x
/*-----------------------------------------------------------------------
|                               Variables                               |
-----------------------------------------------------------------------*/

Switch_t ikey[iSW_LONG]={
{
	.id=0,
	.trigger=0,
	.t.mode=0,
	.shake_time=SHAKE_TIME,

	//optional
	.double_max_time=100,
	.double_min_time=30,
#if 1
	//Mode0
	.t.u.M0.trigger_way=0
#elif 0
	//Mode1
	.t.u.M1.scan_time=60,
	.t.u.M1.max_trigger_cnt=0,
	.t.u.M1.delay_time=2000
#else
	//Mode2
	.t.u.M2.trigger_way=1,
	.t.u.M2.short_time=0,
	.t.u.M2.long_time=2000
#endif
},
{
	//Mast
	.id=1,
	.trigger=0,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
	//optional
	.double_max_time=100,
	.double_min_time=30,
#if 1
	//Mode0
	.t.u.M0.trigger_way=0
	
#elif 0
	//Mode1
	.t.u.M1.scan_time=60,
	.t.u.M1.max_trigger_cnt=0,
	.t.u.M1.delay_time=500
#else
	//Mode2
	.t.u.M2.trigger_way=0,
	.t.u.M2.short_time=0,
	.t.u.M2.long_time=2000
#endif
},
{
	.id=2,
	.trigger=0,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=3,
	.trigger=0,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=4,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=5,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=6,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=7,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=8,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=9,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=10,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=11,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=12,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=13,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=14,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=15,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=16,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=17,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=18,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
},
{
	.id=19,
	.trigger=1,
	.t.mode=0,
	.shake_time=SHAKE_TIME,
}
};




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
void iSW_assert_failed(unsigned char *file, unsigned int line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */



static unsigned char iSWx_Do_Mode0(Switch_t* pSW)
{
	#if ISWITCH_DEBUG==1
	UARTx_Printf(&huart1,"0_1,time%d = %ld\n\r",pSW->id,pSW->scan_status);		
	#endif					
	pSW->scan_status = 100;

	if(pSW->t.u.M0.trigger_way == 0)
		return STATUS_SET(iSW_Mode0_Bit);		

	pSW->t.u.M0.shadow_status = 1<<iSW_Mode0_Bit;
	return 0;
	
}

static unsigned char iSWx_Do_Mode1(Switch_t* pSW)
{
	if(pSW->scan_status == 11)//Mode1_1
	{
		pSW->scan_status = 12;
		return STATUS_SET(iSW_Mode1_Bit);
	}
	else if(pSW->scan_status == 12)//Mode1_2
	{
		if((pSW->status_time) > (pSW->t.u.M1.delay_time))
		{
			if( ((pSW->t.u.M1.trigger_cnt) < (pSW->t.u.M1.max_trigger_cnt))	\
				|| 	(pSW->t.u.M1.max_trigger_cnt == 0)						\
			)
			{
				if((pSW->status_time-pSW->status_time_cnt) >= pSW->t.u.M1.scan_time)
				{
					pSW->t.u.M1.trigger_cnt++;
					#if ISWITCH_DEBUG==1
					UARTx_Printf(&huart1,"1_2,time%d = %d,%dth\n\r",pSW->id,(pSW->status_time),pSW->t.u.M1.trigger_cnt);
					#endif
					pSW->status_time_cnt = pSW->status_time;
					return STATUS_SET(iSW_Mode1_Bit);
				}

			}
			else
			{
				pSW->scan_status = 100;

			}

		}
		
	}
	return 0;
}
static unsigned char iSWx_Do_Mode2(Switch_t* pSW)
{
	if(pSW->scan_status == 21)//Mode2_1
	{
		#if ISWITCH_DEBUG == 1
		UARTx_Printf(&huart1,"2_1,time%d=%d,status=%d\n\r",pSW->id,(pSW->status_time),pSW->status);
		#endif
		if(pSW->status_time >= pSW->t.u.M2.short_time)
		{
			if(pSW->t.u.M2.long_time!=0)
				pSW->scan_status = 22;
			else
				pSW->scan_status = 100;
			
			if(pSW->t.u.M2.trigger_way == 0)
				return STATUS_SET(iSW_Mode2_Short_Bit);
			pSW->t.u.M2.shadow_status = 1<<iSW_Mode2_Short_Bit;
			return 0;
		}

	}
	else if(pSW->scan_status == 22)//Mode2_2
	{
		#if ISWITCH_DEBUG==1
		UARTx_Printf(&huart1,"2_2,time%d=%d,status=%d\n\r",pSW->id,(pSW->status_time),pSW->t.u.M2.shadow_status);
		#endif
		if(pSW->status_time >= pSW->t.u.M2.long_time)
		{
			pSW->scan_status = 100;
			if(pSW->t.u.M2.trigger_way == 0)
				return STATUS_SET(iSW_Mode2_Long_Bit);
			pSW->t.u.M2.shadow_status = 1<<iSW_Mode2_Long_Bit;
			return 0;
		}
	}

	return 0;
}

static unsigned char iSWx_To_Shake(Switch_t* pSW)
{
	
	pSW->status_time++;
	if(pSW->scan_status==0)//SW shake
	{
		pSW->scan_status = 99;
		pSW->scan_time_cnt = pSW->shake_time;//The next scan time is the debounce time
		return 0;
	}
	else if(pSW->scan_status == 99)//First times coming
	{
		#if ISWITCH_DEBUG == 1
		UARTx_Printf(&huart1,"0_0,time%d = %ld\n\r",pSW->id,pSW->status_time);		
		#endif

		if(		pSW->double_min_time != 0				\
			&&  pSW->status_time < pSW->double_max_time	\
			&&  pSW->status_time > pSW->double_min_time	\
		)//double-click tirgger fnuction
		{
//					pSW->double_click++;
			STATUS_SET(iSW_Double_Click_Bit);
		}

		pSW->status_time=0;					
		if(pSW->t.mode == 0)
		{
			pSW->scan_status = 01;
		}
		else if(pSW->t.mode == 1)
		{
			pSW->scan_status = 11;
		}
		else if(pSW->t.mode == 2)
		{
			pSW->scan_status = 21;
		}	
	}
	if(pSW->scan_status == 100)//idle
	{
		return 0;				
	}
	else if(pSW->t.mode == 0)
	{
		iSWx_Do_Mode0(pSW);
	}
	else if(pSW->t.mode == 1)
	{
		iSWx_Do_Mode1(pSW);	
	}
	else if(pSW->t.mode == 2)
	{
		iSWx_Do_Mode2(pSW);
	}
	
	return 0;
}

static unsigned char iSWx_IS_Trigger(unsigned char status,Switch_t* pSW)
{
	if(status==pSW->trigger)
	{
		return iSWx_To_Shake(pSW);
	}
	else//no tirgger
	{
		if(pSW->scan_status > 0)
		{
			if(pSW->t.mode == 0)
			{
				pSW->status |= pSW->t.u.M0.shadow_status;
				pSW->t.u.M0.shadow_status = 0;
			}
			else if(pSW->t.mode == 1)
			{
				pSW->t.u.M1.trigger_cnt = 0;
			}
			else if(pSW->t.mode == 2)
			{
				pSW->status |= pSW->t.u.M2.shadow_status;
				pSW->t.u.M2.shadow_status = 0;
			}
			//Clear MiddleFlag and Values
			pSW->scan_status = 0;
			pSW->status_time_cnt = 0;
			pSW->status_time = 1;//release time count
			return pSW->status;
		}
		pSW->status_time++;	
		return 0;
	}
}



/**
  * @brief  Scan key status and it have max trigger times
  * @param  status  :Key input status
			pSwitch :iSwitch Struct handle
  * @notes  This function requires 1ms clock
  * @retval [0]No trigger,[1]Trigger
  */
unsigned int iSWx_Scan(unsigned char status,Switch_t* pSW)
{
	if(pSW->scan_time_cnt==0)
	{
		return iSWx_IS_Trigger(status,pSW);
	}
	else//waiting......
	{
//		STATUS_RESET(0);
		pSW->scan_time_cnt--;
		return 0;
	}
	
}
/**
  * @brief  Clear All iSW Status
  * @param  pSW :iSwitch Struct handle Array
			size:Array Length
  * @Note 	that this function may cause access to be out of bounds
  * @retval triggered keys (32 bit)
  */
void iSW_Clear(Switch_t *pSW, unsigned int size)
{
	iSW_assert_param(pSW != NULL);
	while(size--)
	{
		(*pSW).status = 0;
		pSW++;
	}
}
/**
  * @brief  Handle scan result and return
  * @param  switch pin status (32 bit)
  * @retval triggered keys (32 bit)
  */
unsigned int iSWx_Handler(unsigned int status32)
{
	unsigned int key_status=0;
	
	for(int i=0;i<iSW_LONG;i++)
	{
		iSWx_Scan((status32&(1<<ikey[i].id))>>ikey[i].id,&ikey[i]);
		if(ikey[i].status > 0)
		{
			key_status |= 1<<ikey[i].id;
		}
	}
	
	return key_status;
}

/**
  * @brief  Get Switch Handle point
  * @param  iSW_id :iSwitch Handle ID
  * @retval Switch Handle point
  */
Switch_t* iSW_Get_Handle(iSW_ID iSW_id)
{
	int i;
	for(i=0;i<iSW_LONG;i++)
	{
		if(ikey[i].id==iSW_id)
			return &ikey[i];
	}
	return NULL;
}

/**
  * @brief  Create a iSW_Mode_t and init 
  * @param  iSW_id :iSwitch Handle ID
  * @param  way .M0 trigger way
  * @retval iSW_Mode_t.M0 handle
  */
iSW_Mode_t iSW_To_Mode0(iSW_ID iSW_id,unsigned char way)
{
	iSW_Mode_t imode;
	imode.mode = 0;
	imode.u.M0.trigger_way = way;
	imode.u.M0.shadow_status = 0;
	iSW_Set_Mode(iSW_id,&imode);
	return imode;
}

/**
  * @brief  Create a iSW_Mode_t and init
  * @param  iSW_id :iSwitch Handle ID
  * @param  delay    :Delay time of press trigger
  * @param  scan_time:Time interval of continuous triggering
  * @param  max_cnt  :Maximum trigger times
  * @retval iSW_Mode_t.M1 handle
  */
iSW_Mode_t iSW_To_Mode1(iSW_ID iSW_id,unsigned int delay,unsigned int scan_time,unsigned int max_cnt)
{
	iSW_Mode_t imode;
	imode.mode = 1;
	imode.u.M1.delay_time = delay;
	imode.u.M1.max_trigger_cnt = max_cnt;
	imode.u.M1.scan_time = scan_time;
	imode.u.M1.trigger_cnt = 0;
	iSW_Set_Mode(iSW_id,&imode);
	return imode;
}

/**
  * @brief  Create a iSW_Mode_t and init
  * @param  iSW_id :iSwitch Handle ID
  * @param  long_time :Long press time 
  * @param  short_time:short press time
  * @param  way       .M2 trigger way[0:immediately][1:until you let go]
  * @retval iSW_Mode_t.M2 handle
  */
iSW_Mode_t iSW_To_Mode2(iSW_ID iSW_id,unsigned int long_time,unsigned int short_time,unsigned char way)
{
	iSW_Mode_t imode;
	imode.mode = 2;
	imode.u.M2.long_time = long_time;
	imode.u.M2.short_time = short_time;
	imode.u.M2.trigger_way = way;
	imode.u.M2.shadow_status = 0;
	iSW_Set_Mode(iSW_id,&imode);
	return imode;
}

unsigned char iSW_Set_Mode(iSW_ID iSW_id,iSW_Mode_t* Mode)
{
	iSW_assert_param(Mode != NULL);
	if(Mode->mode==0||Mode->mode==1||Mode->mode==2)
	{
		Switch_t* pSW = iSW_Get_Handle(iSW_id);
		memcpy(&(pSW->t),Mode,sizeof(iSW_Mode_t));
		return pSW->t.mode;
	}
	return 0xFF;
}

/**
  * @brief  Set Double Click
  * @param  iSW_id :iSwitch Handle ID
  * @param	max    :Maximum waiting time for double-click trigger
  * @param  min    :Minimum waiting time for double-click trigger
  * @retval Current id
  */
unsigned char iSW_Set_Double_Click(iSW_ID id,unsigned char max,unsigned char min)
{
	iSW_assert_param(Mode != NULL);
	Switch_t* pSW = iSW_Get_Handle(id);
	pSW->double_max_time = max;
	pSW->double_min_time = min;
	return pSW->id;
	
}
/**
  * @brief  Get iSW Mode
  * @param  iSW_id :iSwitch Handle ID
  * @retval Current mode
  */
unsigned char iSW_Get_Mode(iSW_ID iSW_id)
{
	Switch_t* pSW = iSW_Get_Handle(iSW_id);
	return pSW->t.mode;
}

/**
  * @brief  Get iSW Status
  * @param  iSW_id :iSwitch Handle ID
  * @retval iSwitch_STATUS
  */
unsigned char iSW_Get_Status(iSW_ID iSW_id,iSW_STATUS Bit)
{
	Switch_t* pSW = iSW_Get_Handle(iSW_id);
	unsigned char ret;
	ret = pSW->scan_status & (1<<Bit);
	STATUS_RESET(Bit);/*Auto Clear Trigger Flag*/
	return ret>>Bit;
}
/**
  * @brief  Get iSW All Status
  * @param  iSW_id :iSwitch Handle ID
  * @retval iSwitch All status
  */
unsigned char iSW_Get_All_Status(iSW_ID iSW_id)
{
	Switch_t* pSW = iSW_Get_Handle(iSW_id);
	unsigned char ret = pSW->status;
	pSW->status = 0;
	return ret;
}

/*Example: How to handle pressing events

	uint8_t *pS = &iSW_Get_Handle(iSW1)->status;
	if(*pS & 1<<0)
		UARTx_Printf(&huart1,"Key1 Mode0\n\r");
	if(*pS & 1<<1)
		UARTx_Printf(&huart1,"Key1 Mode1\n\r");
	if(*pS & 1<<2)
		UARTx_Printf(&huart1,"Key1 Mode2 Short\n\r");
	if(*pS & 1<<3)
		UARTx_Printf(&huart1,"Key1 Mode2 Long\n\r");
	if(*pS & 1<<4)
	{
		UARTx_Printf(&huart1,"Key1 Double Click\n\r");
		iSW_Get_Mode(iSW1)==1 ? iSW_To_Mode2(iSW1,2000,20,0) : iSW_To_Mode1(iSW1,500,60,0);
		UARTx_Printf(&huart1,"Key1当前模式为%d\n\r",iSW_Get_Mode(iSW1));
	}
	*pS = 0;
*/

