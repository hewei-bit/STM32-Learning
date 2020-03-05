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
      ALIENTEKս��STM32������UCOSʵ��
               UCOSIII ����
************************************************/

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK

//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()

//USART�жϷ������ 3,3
//RTC�жϷ������   0,0

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define MAIN_TASK_PRIO		4
//�����ջ��С	
#define MAIN_STK_SIZE 		128
//������ƿ�
OS_TCB Main_TaskTCB;
//�����ջ	
CPU_STK MAIN_TASK_STK[MAIN_STK_SIZE];
void main_task(void *p_arg);


//�������ȼ�
#define TASK1_TASK_PRIO 	5
//�����ջ��С	
#define TASK1_STK_SIZE 	64
//������ƿ�
OS_TCB TASK1_TaskTCB;
//�����ջ	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
//������
void TASK1_task(void *p_arg);

//�������ȼ�
#define TASK2_TASK_PRIO	6
//�����ջ
#define TASK2_STK_SIZE		128
//������ƿ�
OS_TCB	TASK2_TaskTCB;
//�����ջ
CPU_STK	TASK2_TASK_STK[TASK2_STK_SIZE];
//������
void TASK2_task(void *p_arg);

//�������ȼ�
#define RTC_TASK_PRIO	7
//�����ջ
#define RTC_STK_SIZE		128
//������ƿ�
OS_TCB	RTC_TaskTCB;
//�����ջ
CPU_STK	RTC_TASK_STK[TASK2_STK_SIZE];
//������
void rtc_task(void *p_arg);

//�������ȼ�
#define TASK3_TASK_PRIO	8
//�����ջ��С	
#define TASK3_STK_SIZE 	128
//������ƿ�
OS_TCB TASK3_TaskTCB;
//�����ջ	
CPU_STK TASK3_TASK_STK[TASK3_STK_SIZE];
//������
void TASK3_task(void *p_arg);

////////////////////////��ʱ��////////////////////////////////
u8 tmr1sta=0; 	//��Ƕ�ʱ���Ĺ���״̬
OS_TMR	tmr1;	//����һ����ʱ��
void tmr1_callback(void *p_tmr,void *p_arg); //��ʱ��1�ص�����

////////////////////////��Ϣ����//////////////////////////////
#define KEYMSG_Q_NUM	1	//������Ϣ���е�����
#define DATAMSG_Q_NUM	4	//�������ݵ���Ϣ���е�����
OS_Q KEY_Msg;				//����һ����Ϣ���У����ڰ�����Ϣ���ݣ�ģ����Ϣ����
OS_Q DATA_Msg;				//����һ����Ϣ���У����ڷ�������

//////////////////����һ���ź�������������ͬ��///////////////
//OS_SEM	TEST_SEM;	

////////////////////////����һ�������ź���/////////////////////
//OS_MUTEX	TEST_MUTEX;		

////////////////////////�¼���־��//////////////////////////////
#define KEY0_FLAG		0x01
#define KEY1_FLAG		0x02
#define KEYFLAGS_VALUE	0X00						
OS_FLAG_GRP	EventFlags;		//����һ���¼���־��

////////////////////LCDˢ��ʱʹ�õ���ɫ/////////////////
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

//ͼ�ν���
void ucos_main_ui(void)
{
	POINT_COLOR = RED;
	LCD_ShowString(30,10,240,16,16,"Safety Science and Engineering");	
	LCD_ShowString(80,30,200,16,16,"He Wei");
	LCD_ShowString(80,50,200,16,16,"2020/2/13");
	LCD_ShowString(80,70,200,16,16,"KEY_UP:LED1 KEY0:Refresh LCD");
	LCD_ShowString(80,90,200,16,16,"KEY1:Tmr1 KEY2:BEEP");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(200,400,200,16,16,"    -  -  ");	   
	LCD_ShowString(200,432,200,16,16,"  :  :  ");
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(35,110,145,314); 	//��һ������	
	LCD_DrawLine(35,130,145,130);		//����
	
	LCD_DrawRectangle(165,110,285,314); //��һ������	
	LCD_DrawLine(165,130,285,130);		//����
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
	sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//��ʾDATA_Msgʣ���С
	LCD_ShowString(176,240,100,16,16,p);
	myfree(SRAMIN,p);		//�ͷ��ڴ�
	OS_CRITICAL_EXIT();		//�˳��ٽ��
}


//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	uart_init(115200);   //���ڳ�ʼ��
	LED_Init();         //LED��ʼ��	
	LCD_Init();			//LCD��ʼ��	
	KEY_Init();			//������ʼ��
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	RTC_Init();	  			//RTC��ʼ��
	BEEP_Init();		//��ʼ��������
	FSMC_SRAM_Init();	//��ʼ��SRAM
	my_mem_init(SRAMIN);//��ʼ���ڲ�RAM
	
	ucos_main_ui();     //����UI	
	
	OSInit(&err);		    //��ʼ��UCOSIII
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	
				 
	OSStart(&err);      //����UCOSIII
}

//��ʼ����������
void start_task(void *p_arg)//��ʾ��ʱ���øò���
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
	
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	
#if OS_CFG_APP_HOOKS_EN				//ʹ�ù��Ӻ���
	App_OS_SetAllHooks();			
#endif

	OS_CRITICAL_ENTER();	//�����ٽ���
	
//����ģ��
//������ʱ��1
//	OSTmrCreate ((OS_TMR      *)&tmr1 ,   //��ʱ��1
//                 (CPU_CHAR    *)"tmr1",   //��ʱ��1����
//                 (OS_TICK      ) 20   ,   //��һ���� 20*10 = 200ms
//                 (OS_TICK      ) 100  ,   //�������� 100*10 = 1000ms
//                 (OS_OPT       ) OS_OPT_TMR_PERIODIC,//����ģʽ
//                 (OS_TMR_CALLBACK_PTR)tmr1_callback, //��ʱ��1�ص�����
//                 (void        *)0,
//                 (OS_ERR      *)&err);    //���صĴ������
//����һ���ź���
//	OSSemCreate ((OS_SEM*	)&TEST_SEM,
//                 (CPU_CHAR*	)"TEST_SEM",
//                 (OS_SEM_CTR)1,				//�ź�����ʼֵΪ1
//                 (OS_ERR*	)&err);
//����һ�������ź���
//	OSMutexCreate((OS_MUTEX*	)&TEST_MUTEX,
//				  (CPU_CHAR*	)"TEST_MUTEX",
//                  (OS_ERR*		)&err);
//����һ����Ϣ����
//	OSQCreate((OS_Q*       )&KEY_Msg,
//              (CPU_CHAR*   )"KEY Msg",
//              (OS_MSG_QTY  )KEYMSG_Q_NUM,
//              (OS_ERR*     )&err)
//����һ���¼���־��
	OSFlagCreate((OS_FLAG_GRP*)&EventFlags,		//ָ���¼���־��
                 (CPU_CHAR*	  )"Event Flags",	//����
                 (OS_FLAGS	  )KEYFLAGS_VALUE,	//�¼���־���ʼֵ
                 (OS_ERR*  	  )&err);			//������
//����TASK1����
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

	//������Ϣ����KEY_Msg
	OSQCreate ((OS_Q*		)&KEY_Msg,	//��Ϣ����
                (CPU_CHAR*	)"KEY Msg",	//��Ϣ��������
                (OS_MSG_QTY	)KEYMSG_Q_NUM,	//��Ϣ���г��ȣ���������Ϊ1
                (OS_ERR*	)&err);		//������
	//������Ϣ����DATA_Msg
	OSQCreate ((OS_Q*		)&DATA_Msg,	
                (CPU_CHAR*	)"DATA Msg",	
                (OS_MSG_QTY	)DATAMSG_Q_NUM,	
                (OS_ERR*	)&err);
	//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )0,			//0ms
                (OS_TICK	 )50,          //50*10=500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
	//����������
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
	//������������
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
	//����TASK2����
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
	

	//����TASK2����
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
	//����RTC����
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
				 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
				 
	OSTaskDel((OS_TCB*)&StartTaskTCB,&err);	//ɾ��start_task��������
}
/************** ��Ϣ����ʵ��***************/

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
		//��������
		OSQPost((OS_Q*		)&DATA_Msg,		
				(void*		)pbuf,
				(OS_MSG_SIZE)10,
				(OS_OPT		)OS_OPT_POST_FIFO,
				(OS_ERR*	)&err);
		if(err != OS_ERR_NONE)
		{
			myfree(SRAMIN,pbuf);	//�ͷ��ڴ�
			OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ��1
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
		key = KEY_Scan(0);  //ɨ�谴��
		if(key)
		{
			//������Ϣ
			OSQPost((OS_Q*		)&KEY_Msg,		
					(void*		)&key,
					(OS_MSG_SIZE)1,
					(OS_OPT		)OS_OPT_POST_FIFO,
					(OS_ERR*	)&err);
		}
		num++;
		if(num%10==0) check_msg_queque(p);//���DATA_Msg��Ϣ���е�����
		if(num==50)
		{
			num=0;
			LED0 = ~LED0;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //��ʱ10ms
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
				//���¼���־��EventFlags���ͱ�־
				flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
									 (OS_FLAGS	  )KEY0_FLAG,
									 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
									 (OS_ERR*	  )&err);
				LCD_ShowxNum(200,320,flags_num,1,16,0);
				printf("�¼���־��EventFlags��ֵ:%d\r\n",flags_num);
				LED1 = ~LED1; 
				break;
			case KEY2_PRES: BEEP = ~BEEP; break;
			case KEY1_PRES: 
				num++;
				//���¼���־��EventFlags���ͱ�־
				flags_num=OSFlagPost((OS_FLAG_GRP*)&EventFlags,
									 (OS_FLAGS	  )KEY1_FLAG,
									 (OS_OPT	  )OS_OPT_POST_FLAG_SET,
									 (OS_ERR*     )&err);
				LCD_ShowxNum(200,340,flags_num,1,16,0);
				printf("�¼���־��EventFlags��ֵ:%d\r\n",flags_num);
				break;
			case KEY0_PRES:
				tmr1sta = !tmr1sta;
				if(tmr1sta){
					OSTmrStart(&tmr1,&err);
					LCD_ShowString(176,160,100,16,16,"TMR1 START!");
				}
				else{
					OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err); //ֹͣ��ʱ��1
					LCD_ShowString(176,160,100,16,16,"TMR1 STOP! ");
				}
				break;
		}
	}
}	
	
//TASK2������
void TASK2_task(void *p_arg)
{
	u8 *p;
	OS_MSG_SIZE size;
	OS_ERR err;
	p_arg = p_arg;

	while(1)
	{
		//������Ϣ
		p=OSQPend((OS_Q*		)&DATA_Msg,   
				  (OS_TICK		)0,
                  (OS_OPT		)OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE*	)&size,	
                  (CPU_TS*		)0,
                  (OS_ERR*		)&err);
		LCD_ShowString(176,280,100,16,16,p);
		myfree(SRAMIN,p);	//�ͷ��ڴ�
		OSTimeDlyHMSM(0,0,0,700,OS_OPT_TIME_PERIODIC,&err); //��ʱ1s
	}
}

//�¼���־�鴦������
void TASK3_task(void *p_arg)
{
	u8 num;
	OS_ERR err; 
	while(1)
	{
		//�ȴ��¼���־��
		OSFlagPend((OS_FLAG_GRP*)&EventFlags,
				   (OS_FLAGS	)KEY0_FLAG+KEY1_FLAG,
		     	   (OS_TICK     )0,
				   (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
				   (CPU_TS*     )0,
				   (OS_ERR*	    )&err);
		num++;
		LED1 = ~LED1;
		LCD_Fill(36,131,144,313,lcd_discolor[num%14]);
		printf("�¼���־��EventFlags��ֵ:%d\r\n",EventFlags.Flags);
		LCD_ShowxNum(220,340,EventFlags.Flags,1,16,0);
	}
}


//RTCʵʱʱ������
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












/*****************�� 5   �����ʱ��********************/
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
//				printf("������ʱ��1\r\n");
//				break;
//			case KEY0_PRES:
//				OSTmrStart(&tmr2,&err);
//				printf("������ʱ��2\r\n");
//				break;
//			case KEY1_PRES:
//				OSTmrStop(&tmr1,OS_OPT_TMR_NONE,0,&err);
//				OSTmrStop(&tmr2,OS_OPT_TMR_NONE,0,&err);
//				printf("�رն�ʱ��1��2\r\n");
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

////task2������
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
//	tmr2_num++;		//��ʱ��2ִ�д�����1
//	LCD_ShowxNum(232,111,tmr2_num,3,16,0x80);  //��ʾ��ʱ��1ִ�д���
//	LCD_Fill(176,131,283,313,lcd_discolor[tmr2_num%14]); //�������
//	printf("��ʱ��2���н���\r\n");	
//	LED0= ~LED0;
//}

/*****************�� 4 ���к����빳�Ӻ���********************/

/*****************�� 3 ʱ��Ƭ��ת����********************/
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
//		task1_num++;	//����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
//		
//		for(i = 0;i < 5;i++) printf("Task1:01234\r\n");
//		LED0= ~LED0;
//		LCD_ShowxNum(110,130,task1_num,3,16,0x80);	//��ʾ����ִ�д���
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
//	}
//}

//task2������
//void task2_task(void *p_arg)
//{
//	u8 i,task2_num=0;
//	OS_ERR err;
//	p_arg = p_arg;
//	POINT_COLOR = RED;
//	LCD_ShowString(30,150,110,16,16,"Task2 Run:000");

//	while(1)
//	{
//		task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
//		
//		for(i = 0;i < 5;i++) printf("Task2:56789\r\n");
//		LED1=~LED1;
//		LCD_ShowxNum(110,150,task2_num,3,16,0x80);  //��ʾ����ִ�д���
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
//	}
//}

/*****************�� 2 ���������ָ�*******************/
//task1������
//void task1_task(void *p_arg)
//{
//
//	while(1)
//	{
//		task1_num++;	//����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
//		LED0= ~LED0;
//		printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
//		if(task1_num==5) 
//		{
//			OSTaskSuspend((OS_TCB*)&Task2_TaskTCB,&err);	//����1ִ��5�˺�ɾ��������2
//			printf("����1����������2!\r\n");
//		}
//		if(task1_num==10)
//		{
//			OSTaskResume((OS_TCB*)&Task2_TaskTCB,&err);
//			printf("����1�ָ�������2!\r\n");
//		}
//		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //�������
//		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//��ʾ����ִ�д���
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
//		
//	}
//}

////task2������
//void task2_task(void *p_arg)
//{
//	
//	while(1)
//	{
//		task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
//		LED1=~LED1;
//		printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
//		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //��ʾ����ִ�д���
//		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //�������
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
//	}
//}

/*****************�� 1 ���񴴽���ɾ��**********************/
//task1������
//void task1_task(void *p_arg)
//{
//.........................
//	while(1)
//	{
//		task1_num++;	//����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
//		LED0= ~LED0;
//		printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
//		if(task1_num==5) 
//		{
//			OSTaskDel((OS_TCB*)&Task2_TaskTCB,&err);	//����1ִ��5�˺�ɾ��������2
//			printf("����1ɾ��������2!\r\n");
//		}
//		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //�������
//		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//��ʾ����ִ�д���
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
//		
//	}
//}

////task2������
//void task2_task(void *p_arg)
//{
//..............
//	while(1)
//	{
//		task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
//		LED1=~LED1;
//		printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
//		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //��ʾ����ִ�д���
//		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //�������
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
//	}
//}



