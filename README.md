# iSwitch 

## 简介

>iSwitch 是一个针对按键的驱动，它能对按键进行软件去抖，支持多个按键同时按下无冲突，同时拓展了按键的功能，如长按、短按、连续触发、松手触发等。

## 使用要求

- 该驱动需要一个1ms的定时调用，推荐使用RTOS建立一个1ms的按键扫描任务，使用定时器中断也可。
- 无外部依赖文件

## API函数

```C
unsigned int iSWx_Handler(unsigned int status32);
```

- 这是iSwitch运行的核心函数，从IO口读取电平并编码后输入到这里，得到的返回值可用于判断是否有按键触发。

- 该函数不可重入

```C
iSW_Mode_t iSW_To_Mode0(iSW_ID iSW_id,unsigned char way);
```

- 设置一个按键为模式0并切换到该模式
- 参数1：编码ID
- 参数2：`触发方式[0:按下触发][1:松开触发]`

```C
iSW_Mode_t iSW_To_Mode1(iSW_ID iSW_id,unsigned int delay,unsigned int scan_time,unsigned int max_cnt);
```

- 设置一个按键为模式1并切换到该模式
- 参数1：编码ID
- 参数2：`按键按下后连续触发的响应时间建议默认300ms`
- 参数3：`按键连续触发的间隔时间[1,2^32-1](ms)`
- 参数4：`按键最大连按次数[1,2^32-1]/[0]表示无限次连续触发`

```C
iSW_Mode_t iSW_To_Mode2(iSW_ID iSW_id,unsigned int long_time,unsigned int short_time,unsigned char way);
```

- 设置一个按键为模式2并切换到该模式
- 参数1：编码ID
- 参数2：按长按的触发时间
- 参数3：按键短按的触发时间
- 参数4：`触发方式[0:按下触发][1:松开触发]`

```C
unsigned char iSW_Set_Double_Click(iSW_ID id,unsigned char max,unsigned char min);
```

- 设置一个按键的双击模式
- 参数1：编码ID
- 参数2：`双击最大响应阈值[0,255](ms)`
- 参数3：`双击最小响应阈值(0:禁用功能,255]`

```C
unsigned char iSW_Get_Mode(iSW_ID iSW_id);
```

- 获取按键当前的模式
- 参数1：编码ID

```C
Switch_t* iSW_Get_Handle(iSW_ID iSW_id);
```

- 获取当前按键的句柄
- 参数1：编码ID

```C
unsigned char iSW_Get_Status(iSW_ID iSW_id,iSW_STATUS Bit);
```

- 获取当前按键的状态，该函数会自动清理标志位
- 参数1：编码ID
- 参数2：标志位

```C
unsigned char iSW_Get_All_Status(iSW_ID iSW_id)
```

- 获取当前按键的状态的所有标志位，该函数会自动清理标志位
- 参数1：编码ID

```C
void iSW_Clear(Switch_t *pSW, unsigned int size);
```

- 清理按键的标志位，用于批量处理
- `iSW句柄`
- 长度(如果是`iSW`数组的头，可以输入 `iSW_LONG` )，清理单个按键的话输入1

## 使用方法

### 1. 定义按键数量

- 修改 `iSwithc.h` 文件中的 `iSW_LONG` 宏，定义按键的数量(最大数量为`sizeof(int)*8`,对于32位单片机最大32个)

### 2. 配置按键属性

- 在 `iSwitch.c` 文件中找到 `Switch_t ikey[iSW_LONG]` 

  ```c
  Switch_t ikey[iSW_LONG]={
      {
      //Mast-必须配置的选项
      /*编码时的位置[0,31]*/
  	.id=iSW(0),
      /*触发电平(0为低电平)*/
  	.trigger=0,
      /**--------<must>按键模式-------------
  	@Mode0:单触发模式
  	@Mode1:连续触发控制模式
  	@Mode2:短按、长按
       --------------------------------**/	
  	.t.mode=0,
  	.shake_time=SHAKE_TIME,
  
  	//optional-可选的配置选项
  	.double_max_time=100,
  	.double_min_time=30,
      /*模式参数配置，只能选择配置一个*/
  #if 1
  	//Mode0
      /*触发方式[0:按下触发][1:松开触发]*/
  	.t.u.M0.trigger_way=0
  #elif 0
  	//Mode1
      /*按键连续触发的间隔时间[1,2^32-1](ms)*/
  	.t.u.M1.scan_time=60,
      /*按键最大连按次数[1,2^32-1]/[0]表示无限次连续触发*/
  	.t.u.M1.max_trigger_cnt=0,
      /*按键按下后连续触发的响应时间建议默认300ms*/
  	.t.u.M1.delay_time=300
  #else
  	//Mode2
      /*触发方式[0:按下触发][1:松开触发]*/
  	.t.u.M2.trigger_way=1,
      /*按键短按的触发时间*/
  	.t.u.M2.short_time=0,
      /*按长按的触发时间*/
  	.t.u.M2.long_time=2000
  #endif
      }
  }
  ```

  

### 3. 按键信号编码

> 我们知道在32位的C语言编译器中一个 `int` 类型的变量占4个字节，即32个二进制位 `sizeof(int)*8` ，每一个二进制位有两个状态 `0/1`，单片机的数字IO口输入的也是二进制，因此将单片机的IO状态按顺序编排到一个变量中可以节约内存空间。

下面对编码进行举例说明：

```C
/*STM32按键编码函数*/
unsigned int Key_Encode(void)
{
	unsigned int Key_Input = 0;
    /*假设有四个按键，分别是PG0和PG1,PB2和PB7端口，从右到左分别编码到0000 xxxx */
    Key_Input = GPIOG->IDR&(GPIO_PIN_0|GPIO_PIN_1);	//0000 00xx
    Key_Input |= GPIOB->IDR&GPIO_PIN_2;				//0000 0x00
    Key_Input |= GPIOB->IDR&GPIO_PIN_7>>(7-3);		//x000 0000->0000 x000
    return Key_Input;
}
```


```C
/*51编码程序*/
unsigned int Key_Encode(void)
{
	unsigned int Key_Input = 0;
    /*假设有四个按键，分别是P0.0,P0.1和P1.1，P1.2，从右到左分别编码到0000 xxxx*/
    Key_Input = P0^0<<0 | P0^1<<1;		//0000 00xx
    Key_Input |= (P1^1)<<2 | P1^2<<3;	//0000 0xx0->0000 xx00
    return Key_Input;
}
```

编码ID从右到左递增，使用`iSWx`(x∈[0,31])来访问iSW编码ID

### 4. 调用按键扫描任务

假设 `Task_Key_Scan_Callback`是 *按键扫描任务* 的回调函数

简单的按键触发的事件可以在此完成，复杂的事件建议使用队列将消息送到新的任务来处理，避免阻塞

#### 流程如下

- 获取编码后的按键值

- 调用`iSWx_Handler();`得到处理后的按键状态

- 如果返回值大于零说明有按键触发，需逐位判断那个按键

- 找出按键后使用 `iSW_Get_Status()` 得到按键的值，进一步判断触发的方式(单击，双击，长按...)

- 清理标志变量

#### 下面是一个具体例子

```c
void Task_Key_Scan_Callback(void)
{
	unsigned int Key_Input = Key_Encode();
	unsigned int Key_Output = iSWx_Handler(Key_Input);//输出为1表示按键触发(具体是什么类型的触发需要API函数)
    /*下面是应用代码部分*/
	if(Key_Value>0)Beep(50);//有按键按下，蜂鸣器就响一声
	if(Key_Value&0x01)//按键iSW0触发(编码的右边第一个位的按键)
	{
        /*方法一通过得到句柄来访问标志变量*/
		unsigned char *pS = &iSW_Get_Handle(iSW0)->status;//得到iSW按键的句柄
		UARTx_Printf(&huart1,"Key0 Status: %d\n\r",*pS);//将按键按下的状态信息打印到串口1
		if(*pS & 1<<iSW_Mode0_Bit)//判断触发的方式，参考h文件的iSwitch_STATUS枚举体
			UARTx_Printf(&huart1,"F1\n\r");
		if(*pS & 1<<iSW_Mode1_Bit)
			UARTx_Printf(&huart1,"F1 Mode1\n\r");
		if(*pS & 1<<iSW_Mode2_Short_Bit)
			UARTx_Printf(&huart1,"F1 Mode2 Short\n\r");
		if(*pS & 1<<iSW_Mode2_Long_Bit)
			UARTx_Printf(&huart1,"F1 Mode2 Long\n\r");
		if(*pS & 1<<iSW_Double_Click_Bit)
		{
			UARTx_Printf(&huart1,"F1 Double Click\n\r");//双击更改模式的例子
			iSW_Get_Mode(iSW0)==1 ? iSW_To_Mode2(iSW0,2000,20,1) : iSW_To_Mode1(iSW0,500,60,0);
			UARTx_Printf(&huart1,"F1当前模式为%d\n\r",iSW_Get_Mode(iSW0));
		}
		*pS = 0;
	}

	if(Key_Value&0x02)//按键iSW1触发
	{
        /*方法二通过API函数访问标志变量*/
		unsigned char mode_status = iSW_Get_ALL_Status(iSW1);//获得按键触发的状态
		if(mode_status & 1<<iSW_Mode0_Bit)//判断触发方式
			UARTx_Printf(&huart1,"F2\n\r");//执行的操作
		if(mode_status & 1<<iSW_Mode1_Bit)
			UARTx_Printf(&huart1,"F2 Mode1\n\r");
		if(mode_status & 1<<iSW_Mode2_Short_Bit)
			UARTx_Printf(&huart1,"F2 Mode2 Short\n\r");
		if(mode_status & 1<<iSW_Mode2_Long_Bit)
			UARTx_Printf(&huart1,"F2 Mode2 Long\n\r");
		if(mode_status & 1<<iSW_Double_Click_Bit)
		{
			UARTx_Printf(&huart1,"F2 Double Click\n\r");
			iSW_Get_Mode(iSW1)==1 ? iSW_To_Mode2(iSW1,2000,20,0) : iSW_To_Mode1(iSW1,500,60,0);
			UARTx_Printf(&huart1,"F2当前模式为%d\n\r",iSW_Get_Mode(iSW1));
		}
	}

	iSW_Clear(iSW_Get_Handle(iSW0),iSW_LONG);//清空所有按键的标志位

}
```

如果你遇到问题，有不理解的地方，或是有建议，都欢迎在评论或 issue 中提出，让所有人一起讨论。

