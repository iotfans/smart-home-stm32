#include "stm32l4xx_hal.h"
#include "oled.h"
#include "spi.h"
#include "oledfont.h"
#include "stdio.h"

/**************************************************************
*功  能：OLED输入数据
*参  数：dat 数据 cmd 命令
*返回值：无
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
*功  能：设置OLED屏光标位置
*参  数：x：横坐标   y：纵坐标
*返回值：无
**************************************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
} 

/**************************************************************
*功  能：开启OLED显示
*参  数：无
*返回值：无
**************************************************************/    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/**************************************************************
*功  能：关闭OLED显示
*参  数：无
*返回值：无 
**************************************************************/    
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}

/**************************************************************
*功  能：清屏函数
*参  数： 无
*返回值： 无
**************************************************************/  
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(0x00,OLED_DATA); 
		}
	}
}

/**************************************************************
*功  能：涂屏函数
*参  数：byte全屏写同一个字节，写0效果与清屏相同
*返回值：无
**************************************************************/
void OLED_Draw(uint8_t byte)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(byte,OLED_DATA); 
		}
	}
}

/********************************************************
*功能描述：显示BMP图片128×64
*参    数：起始点坐标(x0,y0),终点坐标(x1,y1),x的范围0～127，y为页的范围0～7
*返 回 值：无
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
*功  能：软件初始化SSD1306	
*参  数：无
*返回值：无 
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
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	
	OLED_WR_Byte(0x81,OLED_CMD);	//对比度设置
	OLED_WR_Byte(0xfF,OLED_CMD);	//1~255;默认0X7F (亮度设置,越大越亮)
	
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
函数名：	OLED_ShowChar
功能：	显示单个英文字符
参数：	x,y:字符显示位置起始坐标(y需要为8的倍数)
		num:数值（0-94）
		size:字体大小
返回值：	无
******************************************************************/ 
void OLED_ShowChar(uint16_t x,uint16_t y, uint8_t num, uint8_t size)
{  
	uint8_t pos = 0 , t = 0;	  

	if(size!=16)
	{
		//由于库中字符只提供16X8,强制更正显示字符尺寸为16X8
		size = 16;
	}
	
	num=num-' ';					//得到字符偏移后的值

	//自上到下循环输入
	for(pos=0;pos<size/8;pos++)
	{
		OLED_Set_Pos(x,y+pos);	
		//自左到右循环输入
		for(t=0;t<size/2;t++)
		{
			OLED_WR_Byte(asc2_1608[num][pos*8+t],OLED_DATA);
		}
	}	   	 	  
}

/******************************************************************
函数名：	OLED_ShowString
功能：	显示英文字符串
参数：	x,y :起点坐标
		*p:字符串起始地址
		size:字体大小
返回值：	无
******************************************************************/  
void OLED_ShowString(uint16_t x,uint16_t y,char *p, uint8_t size)
{	 
	//判断是不是非法字符
	while((*p<='~')&&(*p>=' '))	
	{   
		//如果x，y坐标超出预设lcd屏大小则换行显示
		if(x>(128-size/2)) 
		{
			//显示靠前
			x=0;
			//显示换行
			y=(y+size/8)%8;	
		}
		if(y>(8-size/8))
		{
			y=0;
		}
		//循环显示字串符，坐标（x，y）,使用画笔色和背景色，尺寸和模式由参数决定
		OLED_ShowChar(x,y,*p,size);
		//写完一个字符，横坐标加size/2
		x+=size/2;
		//地址自增
		p++;
	}  
} 

/******************************************************************
函数名：	OLED_ShowInt32Num
功能：	显示数字变量值，非32位的变量可以用强制转换（int32_t）输入数值
参数：	x,y :起点坐标
		num:数值范围(-2147483648~2147483647)
		len;显示的最少位数，若显示值的长度大于len正常显示，否则补位显示
		size:字体大小(12,16)
返回值：	无
******************************************************************/
void OLED_ShowInt32Num(uint16_t x,uint16_t y, int32_t num, uint8_t len, uint8_t size)
{		 	
	char show_len[8]={0},show_num[12]={0};
	uint8_t t = 0;
	
	if(len>32)						//len值过大则退出
		return;
	//len设置最少显示位数，例：输出 %6d 
	sprintf(show_len,"%%%dd",len);
	//代入字串符换算，最终完成num2char转换
	sprintf(show_num,show_len,num);
	
	while(*(show_num+t)!=0)			//循环判断是否为结束符
	{
		//显示字串符的数值
		OLED_ShowChar(x+(size/2)*t,y,show_num[t],size);
		//指针偏移地址值自增
		t++;
	}
} 

/******************************************************************
函数名：	OLED_DrawFont16
功能：	显示单个16X16中文字体
参数：	x,y :起点坐标 
		s:字符串地址
返回值：	无
******************************************************************/
void OLED_DrawFont16(uint16_t x, uint16_t y, char *s)
{
	uint8_t x0 = 0, y0 = 0;
	uint16_t k = 0;
	uint16_t HZnum = 0;
	
	//自动统计汉字数目
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	
	
	//循环寻找匹配的Index[2]成员值
	for (k=0;k<HZnum;k++)
	{
		//对应成员值匹配
		if((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			//x方向循环执行写16行，逐行式输入
			for(y0=0;y0<2;y0++)
			{
				OLED_Set_Pos(x,y+y0);
				//每行写入两个字节，自左到右
				for(x0=0;x0<16;x0++)
				{	
					//一次写入1字节
					OLED_WR_Byte(tfont16[k].Msk[y0*16+x0],OLED_DATA);
				}
			}
		//查找到对应点阵关键字完成绘字后立即break退出for循环，防止多个汉字重复取模显示		
		break; 
		}
	}
}

/******************************************************************
函数名：	OLED_DrawFont32
功能：	显示单个32X32中文字体
参数：	x,y :起点坐标	 
		s:字符串地址
返回值：	无
******************************************************************/ 
void OLED_DrawFont32(uint16_t x, uint16_t y, char *s)
{
	uint8_t x0 = 0, y0 = 0;
	uint16_t k = 0;
	uint16_t HZnum = 0;
	
	//自动统计汉字数目
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	
	
	//循环寻找匹配的Index[2]成员值
	for (k=0;k<HZnum;k++)
	{
		//对应成员值匹配
		if((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			//x方向循环执行写16行，逐行式输入
			for(y0=0;y0<4;y0++)
			{
				OLED_Set_Pos(x,y+y0);
				//每行写入两个字节，自左到右
				for(x0=0;x0<32;x0++)
				{	
					//一次写入1字节
					OLED_WR_Byte(tfont32[k].Msk[y0*32+x0],OLED_DATA);
				}
			}
		//查找到对应点阵关键字完成绘字后立即break退出for循环，防止多个汉字重复取模显示		
		break; 
		}
	}
} 

/******************************************************************
函数名：	OLED_Show_Str
功能：	显示一个字符串,包含中英文显示
参数：	x,y :起点坐标
		str :字符串	 
		size:字体大小
返回值：	无
******************************************************************/ 
void OLED_Show_Str(uint16_t x, uint16_t y, char *str,uint8_t size)
{					
	uint16_t x0 = x;
  	uint8_t bHz = 0;				//字符或者中文，首先默认是字符
	
	if(size!=32)
		size=16;					//默认1608
	while(*str!=0)					//判断为否为结束符
	{
		if(!bHz)					//判断是字符
		{
			//如果x，y坐标超出预设lcd屏大小则换行显示
			if(x>(128-size/2)) 
			{
				//显示靠前
				x=0;
				//显示换行
				y=(y+size/8)%8;	
			}
			if(y>(8-size/8))
			{
				y=0;
			}
			if((uint8_t)*str>0x80)	//对显示的字符检查，判断是否为中文
			{
				bHz=1;				//判断为中文，则跳过显示字符改为显示中文
			}
			else			  		//确定为字符
			{		  
				if(*str==0x0D)		//判断是换行符号
				{
					y+=size;		//下一个显示的坐标换行
					x=x0;			//显示靠前
					str++;			//准备下一个字符
				}
				else				//判断不是换行符
				{
					//显示对应尺寸字符
					OLED_ShowChar(x,y,*str,size);
					//显示完后右移起始显示横坐标准备下次显示
					x+=size/2;
				}
				//显示地址自增，准备下一个字符
				str++; 
			}
		}
		else						//判断是中文
		{   
			//如果x，y坐标超出预设lcd屏大小则换行显示
			if(x>(128-size)) 
			{
				//显示靠前
				x=0;
				//显示换行
				y=(y+size/8)%8;	
			}
			if(y>(8-size/8))
			{
				y=0;
			}
			bHz=0;					//改为默认字符用于下次字符判断 
			if(size==32)			//判断是否为32X32大小的中文
				//显示32X32大小的中文
				OLED_DrawFont32(x,y,str);	 	
			else if(size==16)		//否则为16X16大小的中文
				//显示16X16大小的中文
				OLED_DrawFont16(x,y,str);
			//由于显示为中文，需要自增2个地址	
			str+=2;	
			//显示完后右移起始显示横坐标准备下次显示
			x+=size;			
		}						 
	}   
}
