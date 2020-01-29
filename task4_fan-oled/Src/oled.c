#include "stm32l4xx_hal.h"
#include "oled.h"
#include "spi.h"
#include "oledfont.h"
#include "stdio.h"

/**************************************************************
*��  �ܣ�OLED��������
*��  ����dat ���� cmd ����
*����ֵ����
**************************************************************/
void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();
	HAL_SPI_Transmit(&hspi3,&dat,1,10); 		  
	OLED_DC_Set(); 
}

/**************************************************************
*��  �ܣ�����OLED�����λ��
*��  ����x��������   y��������
*����ֵ����
**************************************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
} 

/**************************************************************
*��  �ܣ�����OLED��ʾ
*��  ������
*����ֵ����
**************************************************************/    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/**************************************************************
*��  �ܣ��ر�OLED��ʾ
*��  ������
*����ֵ���� 
**************************************************************/    
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}

/**************************************************************
*��  �ܣ���������
*��  ���� ��
*����ֵ�� ��
**************************************************************/  
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x02,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(0x00,OLED_DATA); 
		}
	}
}

/**************************************************************
*��  �ܣ�Ϳ������
*��  ����byteȫ��дͬһ���ֽڣ�д0Ч����������ͬ
*����ֵ����
**************************************************************/
void OLED_Draw(uint8_t byte)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x02,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(byte,OLED_DATA); 
		}
	}
}

/********************************************************
*������������ʾBMPͼƬ128��64
*��    ������ʼ������(x0,y0),�յ�����(x1,y1),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
*�� �� ֵ����
*********************************************************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;
	if(y1%8==0)
	{
		y=y1/8;  
	}		
	else 
	{
		y=y1/8+1;
	}
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x1;x++)
		{      
			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
		}
	}
}

/**************************************************************
*��  �ܣ������ʼ��SSD1306	
*��  ������
*����ֵ���� 
**************************************************************/				    
void OLED_Init(void)
{ 	
	OLED_RST_Set();
	HAL_Delay(100);
	OLED_RST_Clr();
	HAL_Delay(200);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x02,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	
	OLED_WR_Byte(0x81,OLED_CMD);	//�Աȶ�����
	OLED_WR_Byte(0xfF,OLED_CMD);	//1~255;Ĭ��0X7F (��������,Խ��Խ��)
	
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}  

/******************************************************************
��������	OLED_ShowChar
���ܣ�	��ʾ����Ӣ���ַ�
������	x,y:�ַ���ʾλ����ʼ����(y��ҪΪ8�ı���)
		num:��ֵ��0-94��
		size:�����С
����ֵ��	��
******************************************************************/ 
void OLED_ShowChar(uint16_t x,uint16_t y, uint8_t num, uint8_t size)
{  
	uint8_t pos = 0 , t = 0;	  

	if(size!=16)
	{
		//���ڿ����ַ�ֻ�ṩ16X8,ǿ�Ƹ�����ʾ�ַ��ߴ�Ϊ16X8
		size = 16;
	}
	
	num=num-' ';					//�õ��ַ�ƫ�ƺ��ֵ

	//���ϵ���ѭ������
	for(pos=0;pos<size/8;pos++)
	{
		OLED_Set_Pos(x,y+pos);	
		//������ѭ������
		for(t=0;t<size/2;t++)
		{
			OLED_WR_Byte(asc2_1608[num][pos*8+t],OLED_DATA);
		}
	}	   	 	  
}

/******************************************************************
��������	OLED_ShowString
���ܣ�	��ʾӢ���ַ���
������	x,y :�������
		*p:�ַ�����ʼ��ַ
		size:�����С
����ֵ��	��
******************************************************************/  
void OLED_ShowString(uint16_t x,uint16_t y,char *p, uint8_t size)
{	 
	//�ж��ǲ��ǷǷ��ַ�
	while((*p<='~')&&(*p>=' '))	
	{   
		//���x��y���곬��Ԥ��lcd����С������ʾ
		if(x>(128-size/2)) 
		{
			//��ʾ��ǰ
			x=0;
			//��ʾ����
			y=(y+size/8)%8;	
		}
		if(y>(8-size/8))
		{
			y=0;
		}
		//ѭ����ʾ�ִ��������꣨x��y��,ʹ�û���ɫ�ͱ���ɫ���ߴ��ģʽ�ɲ�������
		OLED_ShowChar(x,y,*p,size);
		//д��һ���ַ����������size/2
		x+=size/2;
		//��ַ����
		p++;
	}  
} 

/******************************************************************
��������	OLED_ShowInt32Num
���ܣ�	��ʾ���ֱ���ֵ����32λ�ı���������ǿ��ת����int32_t��������ֵ
������	x,y :�������
		num:��ֵ��Χ(-2147483648~2147483647)
		len;��ʾ������λ��������ʾֵ�ĳ��ȴ���len������ʾ������λ��ʾ
		size:�����С(12,16)
����ֵ��	��
******************************************************************/
void OLED_ShowInt32Num(uint16_t x,uint16_t y, int32_t num, uint8_t len, uint8_t size)
{		 	
	char show_len[8]={0},show_num[12]={0};
	uint8_t t = 0;
	
	if(len>32)						//lenֵ�������˳�
		return;
	//len����������ʾλ����������� %6d 
	sprintf(show_len,"%%%dd",len);
	//�����ִ������㣬�������num2charת��
	sprintf(show_num,show_len,num);
	
	while(*(show_num+t)!=0)			//ѭ���ж��Ƿ�Ϊ������
	{
		//��ʾ�ִ�������ֵ
		OLED_ShowChar(x+(size/2)*t,y,show_num[t],size);
		//ָ��ƫ�Ƶ�ֵַ����
		t++;
	}
} 

/******************************************************************
��������	OLED_DrawFont16
���ܣ�	��ʾ����16X16��������
������	x,y :������� 
		s:�ַ�����ַ
����ֵ��	��
******************************************************************/
void OLED_DrawFont16(uint16_t x, uint16_t y, char *s)
{
	uint8_t x0 = 0, y0 = 0;
	uint16_t k = 0;
	uint16_t HZnum = 0;
	
	//�Զ�ͳ�ƺ�����Ŀ
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	
	
	//ѭ��Ѱ��ƥ���Index[2]��Աֵ
	for (k=0;k<HZnum;k++)
	{
		//��Ӧ��Աֵƥ��
		if((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			//x����ѭ��ִ��д16�У�����ʽ����
			for(y0=0;y0<2;y0++)
			{
				OLED_Set_Pos(x,y+y0);
				//ÿ��д�������ֽڣ�������
				for(x0=0;x0<16;x0++)
				{	
					//һ��д��1�ֽ�
					OLED_WR_Byte(tfont16[k].Msk[y0*16+x0],OLED_DATA);
				}
			}
		//���ҵ���Ӧ����ؼ�����ɻ��ֺ�����break�˳�forѭ������ֹ��������ظ�ȡģ��ʾ		
		break; 
		}
	}
}

/******************************************************************
��������	OLED_DrawFont32
���ܣ�	��ʾ����32X32��������
������	x,y :�������	 
		s:�ַ�����ַ
����ֵ��	��
******************************************************************/ 
void OLED_DrawFont32(uint16_t x, uint16_t y, char *s)
{
	uint8_t x0 = 0, y0 = 0;
	uint16_t k = 0;
	uint16_t HZnum = 0;
	
	//�Զ�ͳ�ƺ�����Ŀ
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	
	
	//ѭ��Ѱ��ƥ���Index[2]��Աֵ
	for (k=0;k<HZnum;k++)
	{
		//��Ӧ��Աֵƥ��
		if((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			//x����ѭ��ִ��д16�У�����ʽ����
			for(y0=0;y0<4;y0++)
			{
				OLED_Set_Pos(x,y+y0);
				//ÿ��д�������ֽڣ�������
				for(x0=0;x0<32;x0++)
				{	
					//һ��д��1�ֽ�
					OLED_WR_Byte(tfont32[k].Msk[y0*32+x0],OLED_DATA);
				}
			}
		//���ҵ���Ӧ����ؼ�����ɻ��ֺ�����break�˳�forѭ������ֹ��������ظ�ȡģ��ʾ		
		break; 
		}
	}
} 

/******************************************************************
��������	OLED_Show_Str
���ܣ�	��ʾһ���ַ���,������Ӣ����ʾ
������	x,y :�������
		str :�ַ���	 
		size:�����С
����ֵ��	��
******************************************************************/ 
void OLED_Show_Str(uint16_t x, uint16_t y, char *str,uint8_t size)
{					
	uint16_t x0 = x;
  	uint8_t bHz = 0;				//�ַ��������ģ�����Ĭ�����ַ�
	
	if(size!=32)
		size=16;					//Ĭ��1608
	while(*str!=0)					//�ж�Ϊ��Ϊ������
	{
		if(!bHz)					//�ж����ַ�
		{
			//���x��y���곬��Ԥ��lcd����С������ʾ
			if(x>(128-size/2)) 
			{
				//��ʾ��ǰ
				x=0;
				//��ʾ����
				y=(y+size/8)%8;	
			}
			if(y>(8-size/8))
			{
				y=0;
			}
			if((uint8_t)*str>0x80)	//����ʾ���ַ���飬�ж��Ƿ�Ϊ����
			{
				bHz=1;				//�ж�Ϊ���ģ���������ʾ�ַ���Ϊ��ʾ����
			}
			else			  		//ȷ��Ϊ�ַ�
			{		  
				if(*str==0x0D)		//�ж��ǻ��з���
				{
					y+=size;		//��һ����ʾ�����껻��
					x=x0;			//��ʾ��ǰ
					str++;			//׼����һ���ַ�
				}
				else				//�жϲ��ǻ��з�
				{
					//��ʾ��Ӧ�ߴ��ַ�
					OLED_ShowChar(x,y,*str,size);
					//��ʾ���������ʼ��ʾ������׼���´���ʾ
					x+=size/2;
				}
				//��ʾ��ַ������׼����һ���ַ�
				str++; 
			}
		}
		else						//�ж�������
		{   
			//���x��y���곬��Ԥ��lcd����С������ʾ
			if(x>(128-size)) 
			{
				//��ʾ��ǰ
				x=0;
				//��ʾ����
				y=(y+size/8)%8;	
			}
			if(y>(8-size/8))
			{
				y=0;
			}
			bHz=0;					//��ΪĬ���ַ������´��ַ��ж� 
			if(size==32)			//�ж��Ƿ�Ϊ32X32��С������
				//��ʾ32X32��С������
				OLED_DrawFont32(x,y,str);	 	
			else if(size==16)		//����Ϊ16X16��С������
				//��ʾ16X16��С������
				OLED_DrawFont16(x,y,str);
			//������ʾΪ���ģ���Ҫ����2����ַ	
			str+=2;	
			//��ʾ���������ʼ��ʾ������׼���´���ʾ
			x+=size;			
		}						 
	}   
}
