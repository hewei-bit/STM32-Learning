#include "oled.h"
#include "i2c_oled.h"
#include "delay.h"
#include "oledfont.h"  


//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 
uint8_t Bef[3];//����ǰһ�����ݵļ�������1.Ҫд�ڵڼ�ҳ2.0x01Ҫ�ƶ���λ3.дʲô����
uint8_t Cur[3];//��ǰǰһ������1.Ҫд�ڵڼ�ҳ2.0x01Ҫ�ƶ���λ3.дʲô����
//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd) IIC_Write_Data(dat);
	else IIC_Write_Command(dat);
}	


void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
void OLED_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;	 //PB6,PB7�������  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOB6,7
 	GPIO_SetBits(GPIOB,GPIO_Pin_7|GPIO_Pin_6);	//PB6,PB7 �����
	
	delay_ms(200);

	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	OLED_Clear();

//	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
//	OLED_WR_Byte(0x02,OLED_CMD);//---set low column address
//	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
//	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
//	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
//	OLED_WR_Byte(0x81,OLED_CMD); // contract control
//	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
//	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
//	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
//	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
//	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
//	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
//	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
//	OLED_WR_Byte(0x00,OLED_CMD);//
//	
//	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
//	OLED_WR_Byte(0x80,OLED_CMD);//
//	
//	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
//	OLED_WR_Byte(0x05,OLED_CMD);//
//	
//	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
//	OLED_WR_Byte(0xF1,OLED_CMD);//
//	
//	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
//	OLED_WR_Byte(0x12,OLED_CMD);//
//	
//	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
//	OLED_WR_Byte(0x30,OLED_CMD);//
//	
//	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
//	OLED_WR_Byte(0x14,OLED_CMD);//
//	
//	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
//	OLED_Clear();
}
void OLED_Clear(void)
{
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 8/12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc3_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc3_1608[chr][t];	//����1608����
		else if(size==24)temp=asc3_2412[chr][t];	//����2412����
		else if(size==8)temp=asc3_0806[chr][t];
		else return;								//û�е��ֿ�
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
}	   
void OLED_ShowChinese2424(u16 x,u16 y,u8 index,u8 size)
{
        u8 temp,t,t1;
        u16 y0=y;
        u8 *dzk;   
        u8 csize=(size/8+((size%8)?1:0))*size;                                                         //??????
        if(size == 12) dzk=(u8*)Chinese_1212[index];        //???????????? 
        else if(size == 16)        dzk=(u8*)Chinese_1616[index];   //???????????? 
        else if(size == 24) dzk=(u8*)Chinese_2424[index];   //???????????? 
        for(t=0;t<csize;t++)
        {                                                                                                      
                temp=dzk[t];                                                        //??????                          
                for(t1=0;t1<8;t1++)                                                        //?????????????
                {
                        if(temp&0x80)OLED_DrawPoint(x,y,1);
                        else OLED_DrawPoint(x,y,0); 
                        temp<<=1;        
                        y++;
                        if((y-y0)==size)                                                        //?y?????,?y????????24????,x???1
                        {
                                y=y0;
                                x++;
                                break;
                        }
                }           
        }          
}


//������ʾ����
void Before_State_Update(uint8_t y)//����y��ֵ�����ǰһ�����ݵ��йز���
{
	Bef[0]=7-y/8;
	Bef[1]=7-y%8;
	Bef[2]=1<<Bef[1];
}
void Current_State_Update(uint8_t y)//����Yֵ�������ǰ���ݵ��йز���
{
	Cur[0]=7-y/8;//����д�ڵڼ�ҳ
	Cur[1]=7-y%8;//0x01Ҫ�ƶ���λ��
	Cur[2]=1<<Cur[1];//Ҫдʲô����
}

void OLED_DrawWave(uint8_t x,uint8_t y)
{

	int8_t page_sub;
	uint8_t page_buff,i,j;
	Current_State_Update(y);//����Yֵ�������ǰ���ݵ��йز���
	page_sub=Bef[0]-Cur[0];//��ǰֵ��ǰһ��ֵ��ҳ����Ƚ�
	//ȷ����ǰ�У�ÿһҳӦ��дʲô����
	if(page_sub>0)
	{
		page_buff=Bef[0];
//		OLED_SetPos(page_buff,x);
//		WriteDat(Bef[2]-0x01,OLED_DATA);
		OLED_DrawPoint(x,page_buff,1);
		OLED_WR_Byte(Bef[2]-0x01,OLED_DATA);
		page_buff--;
		for(i=0;i<page_sub-1;i++)
		{
//			OLED_SetPos(page_buff,x);
//			WriteDat(0xff);
		OLED_DrawPoint(x,page_buff,1);
		OLED_WR_Byte(0xff,OLED_DATA);
			page_buff--;
		}
//		OLED_SetPos(page_buff,x);
//		WriteDat(0xff<<Cur[1]);
		OLED_DrawPoint(x,page_buff,1);
		OLED_WR_Byte(0xff<<Cur[1],OLED_DATA);
	}
	else if(page_sub==0)
	{
		if(Cur[1]==Bef[1])
		{
//			OLED_SetPos(Cur[0],x);
//			WriteDat(Cur[2]);
			OLED_DrawPoint(x,Cur[0],1);
			OLED_WR_Byte(Cur[2],OLED_DATA);
		}
		else if(Cur[1]>Bef[1])
		{
//			OLED_SetPos(Cur[0],x);
//			WriteDat((Cur[2]-Bef[2])|Cur[2]);
			OLED_DrawPoint(x,Cur[0],1);
			OLED_WR_Byte((Cur[2]-Bef[2])|Cur[2],OLED_DATA);
		}
		else if(Cur[1]<Bef[1])
		{
//			OLED_SetPos(Cur[0],x);
//			WriteDat(Bef[2]-Cur[2]);
			OLED_DrawPoint(x,Cur[0],1);
			OLED_WR_Byte(Bef[2]-Cur[2],OLED_DATA);
		}
	}
	else if(page_sub<0)
	{
		page_buff=Cur[0];
//		OLED_SetPos(page_buff,x);
//		WriteDat((Cur[2]<<1)-0x01);
		OLED_DrawPoint(x,page_buff,1);
		OLED_WR_Byte((Cur[2]<<1)-0x01,OLED_DATA);
		page_buff--;
		for(i=0;i<0-page_sub-1;i++)
		{
//			OLED_SetPos(page_buff,x);
//			WriteDat(0xff);
			OLED_DrawPoint(x,page_buff,1);
			OLED_WR_Byte(0xff,OLED_DATA);
			
			page_buff--;
		}
//		OLED_SetPos(page_buff,x);
//		WriteDat(0xff<<(Bef[1]+1));
		OLED_DrawPoint(x,page_buff,1);
		OLED_WR_Byte(0xff<<(Bef[1]+1),OLED_DATA);
		
	}
	Before_State_Update(y);
	//����һ�У�ÿһҳ�����������
	for(i=0;i<8;i++)
	{
//		OLED_SetPos(i, x+1) ;
		OLED_DrawPoint(x+1,i,1);
		
		for(j=0;j<1;j++)
//			WriteDat(0x00);
			OLED_WR_Byte(0x00,OLED_DATA);
	}
}





//void OLED_Set_Pos(unsigned char x, unsigned char y)
//{
//	OLED_WR_Byte(0xb0+y,OLED_CMD);
//	OLED_WR_Byte((((x+2)&0xf0)>>4)|0x10,OLED_CMD);
//	OLED_WR_Byte(((x+2)&0x0f),OLED_CMD);
//}
//void OLED_ShowCHinese(u8 x,u8 y,u8 no)
//{
//	u8 t,adder=0;
//	OLED_Set_Pos(x,y);	
//    for(t=0;t<16;t++)
//	{
//		OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
//		adder+=1;
//	}	
//	OLED_Set_Pos(x,y+1);	
//    for(t=0;t<16;t++)
//	{	
//		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
//		adder+=1;
//	}	
//}

//void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
//{ 	
//	unsigned int j=0;
//	unsigned char x,y;

//	if(y1%8==0) y=y1/8;      
//	else y=y1/8+1;
//	for(y=y0;y<y1;y++)
//	{
//		OLED_Set_Pos(x0,y);
//		for(x=x0;x<x1;x++)
//		{      
//			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
//		}
//	}
//} 
//void fill_picture(unsigned char fill_Data)
//{
//	unsigned char m,n;
//	for(m=0;m<8;m++)
//	{
//		OLED_WR_Byte(0xb0+m,0);		//page0-page1
//		OLED_WR_Byte(0x02,0);		//low column start address
//		OLED_WR_Byte(0x10,0);		//high column start address
//		for(n=0;n<128;n++)
//		{
//			OLED_WR_Byte(fill_Data,1);
//		}
//	}
//}



