#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include "sram.h"
#include "beep.h"
#include "rtc.h"
#include "usmart.h"	
#include "includes.h"
#include "os_app_hooks.h"

/************************************************
      ALIENTEK战舰STM32开发板UCOS实验
               UCOSIII 任务
************************************************/

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK

//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

//USART中断服务分组 3,3
//RTC中断服务分组   0,0

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define MAIN_TASK_PRIO		4
//任务堆栈大小	
#define MAIN_STK_SIZE 		128
//任务控制块
OS_TCB Main_TaskTCB;
//任务堆栈	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);


//任务优先级
#define TASK1_TASK_PRIO 	5
//任务堆栈大小	
#define TASK1_STK_SIZE 	64
//任务控制块
OS_TCB TASK1_TaskTCB;
//任务堆栈	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
//任务函数
void TASK1_task(void *p_arg);

//任务优先级
#define TASK2_TASK_PRIO	6
//任务堆栈
#define TASK2_STK_SIZE		128
//任务控制块
OS_TCB	TASK2_TaskTCB;
//任务堆栈
CPU_STK	TASK2_TASK_STK[TASK2_STK_SIZE];
//任务函数
void TASK2_task(void *p_arg);

//任务优先级
#define RTC_TASK_PRIO	7
//任务堆栈
#define RTC_STK_SIZE		128
//任务控制块
OS_TCB	RTC_TaskTCB;
//任务堆栈
CPU_STK	RTC_TASK_STK[TASK2_STK_SIZE];
//任务函数
void rtc_task(void *p_arg);

//任务优先级
#define TASK3_TASK_PRIO	8
//任务堆栈大小	
#define TASK3_STK_SIZE 	128
//任务控制块
OS_TCB TASK3_TaskTCB;
//任务堆栈	
CPU_STK TASK3_TASK_STK[TASK3_STK_SIZE];
//任务函数
void TASK3_task(void *p_arg);

////////////////////////定时器////////////////////////////////
u8 tmr1sta=0; 	//标记定时器的工作状态
OS_TMR	tmr1;	//定义一个定时器
void tmr1_callback(void *p_tmr,void *p_arg); //定时器1回调函数

////////////////////////消息队列//////////////////////////////
#define KEYMSG_Q_NUM	1	//按键消息队列的数量
#define DATAMSG_Q_NUM	4	//发送数据的消息队列的数量
OS_Q KEY_Msg;				//定义一个消息队列，用于按键消息传递，模拟消息邮箱
OS_Q DATA_Msg;				//定义一个消息队列，用于发送数据

//////////////////定义一个信号量，用于任务同步///////////////
//OS_SEM	TEST_SEM;	

////////////////////////定义一个互斥信号量/////////////////////
//OS_MUTEX	TEST_MUTEX;		

////////////////////////事件标志组//////////////////////////////
#define KEY0_FLAG		0x01
#define KEY1_FLAG		0x02
#define KEYFLAGS_VALUE	0X00						
OS_FLAG_GRP	EventFlags;		//定义一个事件标志组

////////////////////LCD刷屏时使用的颜色/////////////////
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

//图形界面
void ucos_main_ui(void)
{
	POINT_COLOR = RED;
	LCD_ShowString(30,10,240,16,16,"Safety Science and Engineering");	
	LCD_ShowString(80,30,200,16,16,"He Wei");
	LCD_ShowString(80,50,200,16,16,"2020/2/13");
	LCD_ShowString(80,70,200,16,16,"KEY_UP:LED1 KEY0:Refresh LCD");
	LCD_ShowString(80,90,200,16,16,"KEY1:Tmr1 KEY2:BEEP");
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(200,400,200,16,16,"    -  -  ");	   
	LCD_ShowString(200,432,200,16,16,"  :  :  ");
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(35,110,145,314); 	//画一个矩形	
	LCD_DrawLine(35,130,145,130);		//画线
	
	LCD_DrawRectangle(165,110,285,314); //画一个矩形	
	LCD_DrawLine(165,130,285,130);		//画线
	POINT_COLOR = BLUE;
	LCD_ShowString(58,112,110,16,16,"Reslut ");
	LCD_ShowString(176,112,110,16,16,"Working");
	POINT_COLOR = BLUE;
	LCD_ShowString(176,140,100,16,16,"tmr1 state:");
	LCD_ShowString(176,180,120,16,16,"DATA_Msg Size:");
	LCD_ShowString(176,220,120,16,16,"DATA_Msg rema:");
	LCD_ShowString(176,260,100,16,16,"DATA_Msg:");
	POINT_COLOR = RED;
	LCD_ShowString(176,160,100,16,16,"TMR1 STOP! ");
}

//
void check_msg_queque(u8 *p)
{
	CPU_SR_ALLOC();
	u8 msgq_remain_size;
	
	OS_CRITICAL_ENTER();
	msgq_remain_size = DATA_Msg.MsgQ.NbrEntriesSize - DATA_Msg.MsgQ.NbrEntries;
	p = mymalloc(SRAMIN,20);
    sprintf((char*)p,"Total Size:%d",DATA_Msg.MsgQ.NbrEntriesSize);
	LCD_ShowString(176,200,100,16,16,p);
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//显示DATA_Msg剩余大小
	LCD_ShowString(176,240,100,16,16,p);
	myfree(SRAMIN,p);		//释放内存
	OS_CRITICAL_EXIT();		//退出临界段
}


//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();  //时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	uart_init(115200);   //串口初始化
	LED_Init();         //LED初始化	
	LCD_Init();			//LCD初始化	
	KEY_Init();			//按键初始化
	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART	
	RTC_Init();	  			//RTC初始化
	BEEP_Init();		//初始化蜂鸣器
	FSMC_SRAM_Init();	//初始化SRAM
	my_mem_init(SRAMIN);//初始化内部RAM
	
	ucos_main_ui();     //加载UI	
	
	OSInit(&err);		    //初始化UCOSIII
	
	OS_CRITICAL_ENTER();	//进入临界区
	
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	
				 
	OSStart(&err);      //开启UCOSIII
}

//开始任务任务函数
void start_task(void *p_arg)//表示暂时不用该参数
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
	
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
#if OS_CFG_APP_HOOKS_EN				//使用钩子函数
	App_OS_SetAllHooks();			
#endif

	OS_CRITICAL_ENTER();	//进入临界区
	
//创建模板
//创建定时器1
//	OSTmrCreate ((OS_TMR      *)&tmr1 ,   //定时器1
//                 (CPU_CHAR    *)"tmr1",   //定时器1名字
//                 (OS_TICK      ) 20   ,   //第一周期 20*10 = 200ms
//                 (OS_TICK      ) 100  ,   //后续周期 100*10 = 1000ms
//                 (OS_OPT       ) OS_OPT_TMR_PERIODIC,//周期模式
//                 (OS_TMR_CALLBACK_PTR)tmr1_callback, //定时器1回调函数
//                 (void        *)0,
//                 (OS_ERR      *)&err);    //返回的错误代码
//创建一个信号量
//	OSSemCreate ((OS_SEM*	)&TEST_SEM,
//                 (CPU_CHAR*	)"TEST_SEM",
//                 (OS_SEM_CTR)1,				//信号量初始值为1
//                 (OS_ERR*	)&err);
//创建一个互斥信号量
//	OSMutexCreate((OS_MUTEX*	)&TEST_MUTEX,
//				  (CPU_CHAR*	)"TEST_MUTEX",
//                  (OS_ERR*		)&err);
//创建一个消息队列
//	OSQCreate((OS_Q*       )&KEY_Msg,
//              (CPU_CHAR*   )"KEY Msg",
//              (OS_MSG_QTY  )KEYMSG_Q_NUM,
//              (OS_ERR*     )&err)
//创建一个事件标志组
	OSFlagCreate((OS_FLAG_GRP*)&EventFlags,		//指向事件标志组
                 (CPU_CHAR*	  )"Event Flags",	//名字
                 (OS_FLAGS	  )KEYFLAGS_VALUE,	//事件标志组初始值
                 (OS_ERR*  	  )&err);			//错误码
//创建TASK1任务
//	OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
//				 (CPU_CHAR	* )"Task1 task", 		
//                 (OS_TASK_PTR )task1_task, 			
//                 (void		* )0,					
//                 (OS_PRIO	  )TASK1_TASK_PRIO,     
//                 (CPU_STK   * )&TASK1_TASK_STK[0],	
//                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
//                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
//                 (OS_MSG_QTY  )0,					
//                 (OS_TICK	  )3,					
//                 (void   	* )0,					
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
//                 (OS_ERR 	* )&err);				

	//创建消息队列KEY_Msg
	OSQCreate ((OS_Q*		)&KEY_Msg,	//消息队列
                (CPU_CHAR*	)"KEY Msg",	//消息队列名称
                (OS_MSG_QTY	)KEYMSG_Q_NUM,	//消息队列长度，这里设置为1
                (OS_ERR*	)&err);		//错误码
	//创建消息队列DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)DATAMSG_Q_NUM,	
                (OS_ERR*	)&err);
	//创建定时器1
	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
                (CPU_CHAR	*)"tmr1",		//定时器名字
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )50,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
                (void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
	//创建主任务
	OSTaskCreate((OS_TCB 	* )&Main_TaskTCB,		
				 (CPU_CHAR	* )"Main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     
                 (CPU_STK   * )&MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
	//创建按键任务
	OSTaskCreate((OS_TCB 	* )&TASK1_TaskTCB,		
				 (CPU_CHAR	* )"TASK1 task", 		
                 (OS_TASK_PTR )TASK1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK1_TASK_PRIO,     
                 (CPU_STK   * )&TASK1_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
	//创建TASK2任务
	OSTaskCreate((OS_TCB 	* )&TASK2_TaskTCB,		
				 (CPU_CHAR	* )"TASK2 task", 		
                 (OS_TASK_PTR )TASK2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK2_TASK_PRIO,     
                 (CPU_STK   * )&TASK2_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	

	//创建TASK2任务
	OSTaskCreate((OS_TCB*     )&TASK3_TaskTCB,		
				 (CPU_CHAR*   )"TASK3 task", 		
                 (OS_TASK_PTR )TASK3_task, 			
                 (void* 	  )0,					
                 (OS_PRIO	  )TASK3_TASK_PRIO,     
                 (CPU_STK* 	  )&TASK3_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK3_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK3_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void* 	  )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR* 	  )&err);
	//创建RTC任务
	OSTaskCreate((OS_TCB 	* )&RTC_TaskTCB,		
				 (CPU_CHAR	* )"RTC task", 		
                 (OS_TASK_PTR )rtc_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )RTC_TASK_PRIO,     
                 (CPU_STK   * )&RTC_TASK_STK[0],	
                 (CPU_STK_SIZE)RTC_STK_SIZE/10,	
                 (CPU_STK_SIZE)RTC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OS_CRITICAL_EXIT();	//退出临界区
				 
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);	//删除start_task任务自身
}
/************** 消息传递实验***************/

void tmr1_callback(void *p_tmr,void *p_arg)
{
	u8 *pbuf;
	static u8 msg_num;
	OS_ERR err;
	pbuf =mymalloc(SRAMIN,10);
	if(pbuf)
	{
		msg_num++;
		sprintf((char*)pbuf,"ALIENTEK %d",msg_num);
		//发送请求
		OSQPost((OS_Q*		)&DATA_Msg,		
				(void*		)pbuf,
				(OS_MSG_SIZE)10,
				(OS_OPT		)OS_OPT_POST_FIFO,
				(OS_ERR*	)&err);
		if(err != OS_ERR_NONE)
		{
			myfree(SRAMIN,pbuf);	//释放内存
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //停止定时器1
			tmr1sta = !tmr1sta;
			LCD_ShowString(176,160,100,16,16,"TMR1 STOP! ");
		}
	}
}

void main_task(void *p_arg)
{
	u8 key,num;
	OS_ERR err;
	u8 *p;
	while(1)
	{
		key = KEY_Scan(0);  //扫描按键
		if(key)
		{
			//发送消息
			OSQPost((OS_Q*		)&KEY_Msg,		
					(void*		)&key,
					(OS_MSG_SIZE)1,
					(OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
		}
		num++;
		if(num%10==0) check_msg_queque(p);//检查DATA_Msg消息队列的容量
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //延时10ms
	}
}

void TASK1_task(void *p_arg)
{
	OS_FLAGS flags_num;
	u8 num;
	u8 *key;
	OS_MSG_SIZE size;
	OS_ERR err;
	
	while(1)
	{
		key = OSQPend((OS_Q*       )&KEY_Msg,
					  ( OS_TICK    )0,
					  (OS_OPT      )OS_OPT_PEND_BLOCKING,
					  (OS_MSG_SIZE*)&size,
					  (CPU_TS*     )0,
					  (OS_ERR*     )&err);
		switch(*key)
		{
			case WKUP_PRES: 
				//向事件标志组EventFlags发送标志
				flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
									 (OS_FLAGS	  )KEY0_FLAG,
									 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
									 (OS_ERR*	  )&err);
				LCD_ShowxNum(200,320,flags_num,1,16,0);
				printf("事件标志组EventFlags的值:%d\r\n",flags_num);
				LED1 = ~LED1; 
				break;
			case KEY2_PRES: BEEP = ~BEEP; break;
			case KEY1_PRES: 
				num++;
				//向事件标志组EventFlags发送标志
				flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
									 (OS_FLAGS	  )KEY1_FLAG,
									 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
									 (OS_ERR*     )&err);
				LCD_ShowxNum(200,340,flags_num,1,16,0);
				printf("事件标志组EventFlags的值:%d\r\n",flags_num);
				break;
			case KEY0_PRES:
				tmr1sta = !tmr1sta;
				if(tmr1sta){
					OSTmrStart(&tmr1,&err);
					LCD_ShowString(176,160,100,16,16,"TMR1 START!");
				}
				else{
					OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //停止定时器1
					LCD_ShowString(176,160,100,16,16,"TMR1 STOP! ");
				}
				break;
		}
	}
}	
	
//TASK2任务函数
void TASK2_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err;
	p_arg = p_arg;

	while(1)
	{
		//请求消息
		p=OSQPend((OS_Q*		)&DATA_Msg,   
				  (OS_TICK		)0,
                  (OS_OPT		)OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE*	)&size,	
                  (CPU_TS*		)0,
                  (OS_ERR*		)&err);
		LCD_ShowString(176,280,100,16,16,p);
		myfree(SRAMIN,p);	//释放内存
		OSTimeDlyHMSM(0,0,0,700,OS_OPT_TIME_PERIODIC,&err); //延时1s
	}
}

//事件标志组处理任务
void TASK3_task(void *p_arg)
{
	u8 num;
	OS_ERR err; 
	while(1)
	{
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&EventFlags,
				   (OS_FLAGS	)KEY0_FLAG+KEY1_FLAG,
		     	   (OS_TICK     )0,
				   (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
				   (CPU_TS*     )0,
				   (OS_ERR*	    )&err);
		num++;
		LED1 = ~LED1;
		LCD_Fill(36,131,144,313,lcd_discolor[num%14]);
		printf("事件标志组EventFlags的值:%d\r\n",EventFlags.Flags);
		LCD_ShowxNum(220,340,EventFlags.Flags,1,16,0);
	}
}


//RTC实时时钟任务
void rtc_task(void *p_arg)
{
	u8 t = 0;
	OS_ERR err;
	while(1)
	{								    
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(200,400,calendar.w_year,4,16);									  
			LCD_ShowNum(240,400,calendar.w_month,2,16);									  
			LCD_ShowNum(264,400,calendar.w_date,2,16);	 
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(200,418,200,16,16,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(200,418,200,16,16,"Monday   ");
					break;
				case 2:
					LCD_ShowString(200,418,200,16,16,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(200,418,200,16,16,"Wednesday");
					break;
				case 4:
					LCD_ShowString(200,418,200,16,16,"Thursday ");
					break;
				case 5:
					LCD_ShowString(200,418,200,16,16,"Friday   ");
					break;
				case 6:
					LCD_ShowString(200,418,200,16,16,"Saturday ");
					break;  
			}
			LCD_ShowNum(200,432,calendar.hour,2,16);									  
			LCD_ShowNum(224,432,calendar.min,2,16);									  
			LCD_ShowNum(248,432,calendar.sec,2,16);
		}	
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);
	}
}












/*****************例 5   软件定时器********************/
//void task1_task(void *p_arg)
//{
//	u8 key,num;
//	OS_ERR err;
//	p_arg = p_arg;
//	
//	while(1)
//	{
//		key = KEY_Scan(0);
//		switch(key)
//		{
//			case WKUP_PRES:
//				OSTmrStart(&tmr1,&err);
//				printf("开启定时器1\r\n");
//				break;
//			case KEY0_PRES:
//				OSTmrStart(&tmr2,&err);
//				printf("开启定时器2\r\n");
//				break;
//			case KEY1_PRES:
//				OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err);
//				OSTmrStop(&tmr2,OS_OPT_TMR_NONE,0,&err);
//				printf("关闭定时器1和2\r\n");
//				break;
//		}
//		num++;
//		if(num==50)
//		{
//			num = 0;
//			LED0= ~LED0;
//		}
//		OSTimeDlyHMSM(0,0,0,1000,OS_OPT_TIME_PERIODIC,&err);
//	}
//}

////task2任务函数
//void task2_task(void *p_arg)
//{
//	OS_ERR err;
//	p_arg = p_arg;

//	while(1)
//	{
//		LED1=~LED1;	
//		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_PERIODIC,&err);
//	}
//}

//void tmr1_callback(void *p_tmr, void *p_arg)
//{
//	static u8 tmr1_num = 0;
//	LCD_ShowxNum(112,111,tmr1_num,3,16,0x80);
//	LCD_Fill(56,131,164,313,lcd_discolor[tmr1_num%14]);
//	tmr1_num++;
//	BEEP =~BEEP;
//}

//void tmr2_callback(void *p_tmr, void *p_arg)
//{
//	static u8 tmr2_num = 0;
//	tmr2_num++;		//定时器2执行次数加1
//	LCD_ShowxNum(232,111,tmr2_num,3,16,0x80);  //显示定时器1执行次数
//	LCD_Fill(176,131,283,313,lcd_discolor[tmr2_num%14]); //填充区域
//	printf("定时器2运行结束\r\n");	
//	LED0= ~LED0;
//}

/*****************例 4 空闲函数与钩子函数********************/

/*****************例 3 时间片轮转调度********************/
//void task1_task(void *p_arg)
//{
//	u8 i,task1_num=0;
//	OS_ERR err;
//	p_arg = p_arg;
//	
//	POINT_COLOR = BLUE;
//	LCD_ShowString(30,130,110,16,16,"Task1 Run:000");

//	while(1)
//	{
//		task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
//		
//		for(i = 0;i < 5;i++) printf("Task1:01234\r\n");
//		LED0= ~LED0;
//		LCD_ShowxNum(110,130,task1_num,3,16,0x80);	//显示任务执行次数
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
//	}
//}

//task2任务函数
//void task2_task(void *p_arg)
//{
//	u8 i,task2_num=0;
//	OS_ERR err;
//	p_arg = p_arg;
//	POINT_COLOR = RED;
//	LCD_ShowString(30,150,110,16,16,"Task2 Run:000");

//	while(1)
//	{
//		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
//		
//		for(i = 0;i < 5;i++) printf("Task2:56789\r\n");
//		LED1=~LED1;
//		LCD_ShowxNum(110,150,task2_num,3,16,0x80);  //显示任务执行次数
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
//	}
//}

/*****************例 2 任务挂起与恢复*******************/
//task1任务函数
//void task1_task(void *p_arg)
//{
//
//	while(1)
//	{
//		task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
//		LED0= ~LED0;
//		printf("任务1已经执行：%d次\r\n",task1_num);
//		if(task1_num==5) 
//		{
//			OSTaskSuspend((OS_TCB*)&Task2_TaskTCB,&err);	//任务1执行5此后删除掉任务2
//			printf("任务1挂起了任务2!\r\n");
//		}
//		if(task1_num==10)
//		{
//			OSTaskResume((OS_TCB*)&Task2_TaskTCB,&err);
//			printf("任务1恢复了任务2!\r\n");
//		}
//		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
//		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//显示任务执行次数
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
//		
//	}
//}

////task2任务函数
//void task2_task(void *p_arg)
//{
//	
//	while(1)
//	{
//		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
//		LED1=~LED1;
//		printf("任务2已经执行：%d次\r\n",task2_num);
//		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //显示任务执行次数
//		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
//	}
//}

/*****************例 1 任务创建与删除**********************/
//task1任务函数
//void task1_task(void *p_arg)
//{
//.........................
//	while(1)
//	{
//		task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
//		LED0= ~LED0;
//		printf("任务1已经执行：%d次\r\n",task1_num);
//		if(task1_num==5) 
//		{
//			OSTaskDel((OS_TCB*)&Task2_TaskTCB,&err);	//任务1执行5此后删除掉任务2
//			printf("任务1删除了任务2!\r\n");
//		}
//		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
//		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//显示任务执行次数
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
//		
//	}
//}

////task2任务函数
//void task2_task(void *p_arg)
//{
//..............
//	while(1)
//	{
//		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
//		LED1=~LED1;
//		printf("任务2已经执行：%d次\r\n",task2_num);
//		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //显示任务执行次数
//		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
//	}
//}



