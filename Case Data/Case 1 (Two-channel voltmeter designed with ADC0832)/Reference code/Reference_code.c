/***********************************************
代码名称：模数转换器ADC0832 A/D转换例程（29、基于51单片机的数字电压表（ADC0832））
功能说明：液晶1602显示ADC0832 两路A/D转换结果 显示电压值 0-5V范围
接口说明：RS     P2^5;	
		  RW  	 P2^6;
		  E  	 P2^7;	
		  数据口 P0 
		  ADCLK	 P1^0;	  
		  ADDO   P1^1;
		  ADDI   P1^1;
		  ADCS   P1^3;
编译环境：keil4
MCU     ：STC89C52RC
时钟频率: 11.0592MHz
作者    ：康微电子工作室
淘宝店铺：https://shop129920945.taobao.com
***********************************************/
#include "reg52.h"
#include "intrins.h"

	
#define LcdDataPort P0   //数据位宏定义

sbit RS=P2^5;	 	 	 //数据命令选择端
sbit RW=P2^6;            //读写选择端
sbit EN=P2^7;            //使能信号端
/************************************************************************
* 函数: void delay_ms(unsigned int nms)	
* 描述: 延时函数，延时nms毫秒   11.0592MHZ
* 参数: nms :  毫秒
* 返回: none.
************************************************************************/
void delay_ms(unsigned int nms)		
{
	unsigned int i ,j;
	for(i=nms;i>0;i--)              //nms即延时约n毫秒 
	{	for(j=110;j>0;j--);	}
}
/************************************************************************
* 函数: void delay_10us(unsigned int nus) 
* 描述: 延时函数，延时nus*10微秒    非精准延时 
* 参数: nus :  微秒
* 返回: none.
************************************************************************/
void delay_10us(unsigned int nus) 
{
	while(nus--);
}

/******************************************************************************

 ******************************************************************************
*  文 件 名   : LCD1602.c
lcd1602子程序
******************************************************************************/
/************************************************************************
* 函数: bit Lcd1602_Check_Busy(void)
* 描述: 查忙函数
* 参数: none.
* 返回: 1 ：忙
		0 ：不忙
************************************************************************/    
bit Lcd1602_Check_Busy(void)      		//查忙函数
{  
   	register unsigned char busy;
	RS=0;				  				//读状态时 RS = 0 RW = 1..EN = 1;
	RW=1;
	EN=1;
	delay_10us(30);
	busy=LcdDataPort;
	EN=0;
	return(busy&0x80);	 
}
/************************************************************************
* 函数: void Lcd1602_Lcd1602_Write_Com(unsigned char com)
* 描述: 液晶写命令
* 参数: com : 要写的命令
* 返回: none.
************************************************************************/
void Lcd1602_Write_Com(unsigned char com)	
{
    while(Lcd1602_Check_Busy());		//忙则等待
	RS=0;								//写指令时 RS = 0 RW = 0 ..EN 1->0		  
	RW=0;
	EN=1;	
	LcdDataPort=com;
	EN=0;
} 
/************************************************************************
* 函数: void Lcd1602_Write_Data(unsigned char dat)
* 描述: 液晶写数据  要显示什么字符就写入
* 参数: dat ：要写的数据
* 返回: none. 
************************************************************************/
void Lcd1602_Write_Data(unsigned char dat)	
{
	while(Lcd1602_Check_Busy());		//忙则等待
	RS=1;					   			//写数据时 RS = 1 RW = 0 ..EN 1->0
	RW=0;
	EN=1;	
	LcdDataPort=dat;
	EN=0; 
}
/************************************************************************
* 函数: void write_string(unsigned char x,unsigned char y,unsigned char *string)
* 描述: 显示字符串函数
* 参数: x : x坐标
		y : y坐标
		*string : 要显示的字符串
* 返回: none.
************************************************************************/
void Lcd1602_String(unsigned char x,unsigned char y,unsigned char *string)	
{
	if(y==0)                  			 //设置行
		Lcd1602_Write_Com(0x80+x);       //第一行0x80开头
	else
		Lcd1602_Write_Com(0xc0+x);		 //第二行0xc0开头
	while(*string)                       //字符串传地址
	{
		Lcd1602_Write_Data(*string);
		string++;
	}
}
/************************************************************************
* 函数: void Lcd_Init(void)	
* 描述: LCD1602液晶初始化
* 参数: none.
* 返回: none. 
************************************************************************/							 
void Lcd1602_Init(void)		  //LCD1602液晶初始化
{
	 Lcd1602_Write_Com(0x38); //设置显示模式 16*2显示 5*7点阵 8位数据接口
	 Lcd1602_Write_Com(0x0c); //显示器开、不显示光标、光标不显示
	 Lcd1602_Write_Com(0x06); //写入一个字符后地址自动加1、屏幕不移动
	 Lcd1602_Write_Com(0x01); //清屏光标复位
}




sbit ADCLK =P1^0;	//AD0832时钟接口
sbit ADDI = P1^1;	//AD0832数据信号输入，选择通道控制
sbit ADDO = P1^1;	//AD0832数据信号输出，转换数据输出
sbit ADCS = P1^3;	//AD0832片选接口


/************************************************************************
* 函数: unsigned char ADC0832(bit mode,bit channel)
* 描述: ADC0832 转换函数
* 参数: mode　 ：低电平为差分模式，高电平为单通道模式。
		channel：通道　　0，1　　　
* 返回: 0：转换失败　　其他：为A/D转换的值 
* 备注: A/D转换的值范围为0-256   8位A/D  即2的8次方=256
************************************************************************/    
unsigned char ADC0832(bit mode,bit channel) //AD转换，返回结果
{
	unsigned char i,dat,ndat;
	ADCS = 0;//拉低CS端
	_nop_();
	_nop_();
	ADDI = 1; //第1个下降沿为高电平
	ADCLK = 1;//拉高CLK端
	_nop_();
	_nop_();
	ADCLK = 0;//拉低CLK端,形成下降沿1
	_nop_();
	_nop_();
	ADDI = mode; //低电平为差分模式，高电平为单通道模式。
	ADCLK = 1;//拉高CLK端
	_nop_();
	_nop_();
	ADCLK = 0;//拉低CLK端,形成下降沿2
	_nop_();
	_nop_();
	ADDI = channel; //低电平为CH0，高电平为CH1
	ADCLK = 1;//拉高CLK端
	_nop_();
	_nop_();
	ADCLK = 0;//拉低CLK端,形成下降沿3
	ADDI = 1;//控制命令结束(经试验必需)
	dat = 0;
	//下面开始读取转换后的数据，从最高位开始依次输出（D7~D0） 
	for(i = 0;i < 8;i++)
	{
		dat <<= 1;
		ADCLK=1;//拉高时钟端
		_nop_();
		_nop_();
		ADCLK=0;//拉低时钟端形成一次时钟脉冲
		_nop_();
		_nop_();
		dat |= ADDO;
	}
	ndat = 0; //记录D0
	if(ADDO == 1)
		ndat |= 0x80;
	//下面开始继续读取反序的数据（从D1到D7）
	for(i = 0;i < 7;i++)
	{
		ndat >>= 1;
		ADCLK = 1;//拉高时钟端
		_nop_();
		_nop_();
		ADCLK=0;//拉低时钟端形成一次时钟脉冲
		_nop_();
		_nop_();
		if(ADDO==1)
		ndat |= 0x80;
	}
	ADCS=1;//拉高CS端,结束转换
	ADCLK=0;//拉低CLK端
	ADDI=1;//拉高数据端,回到初始状态
	if(dat==ndat)
		return(dat);
	else
		return 0;
}



	
void main(void)
{
	unsigned char adc0,adc1;
	unsigned char num=0;
	unsigned char ADC_Get[10]={0};  		//定义AD采样数组

	float Voltage0 = 0.0;
	float Voltage1 = 0.0;

	Lcd1602_Init();			  				//LCD1602液晶初始化

	Lcd1602_String(0,0,"V0:         ");
	Lcd1602_String(0,1,"V1:         ");
	while(1)
	{

		adc0=ADC0832(1,0);			//通道0  
		Voltage0 = (float)adc0 * 5.0/255;
		Voltage0 = Voltage0 *100;				//扩大100倍方便显示
		Lcd1602_Write_Com(0x80+3);
		Lcd1602_Write_Data((unsigned int)Voltage0/100+0x30);
		Lcd1602_Write_Data('.');
		Lcd1602_Write_Data((unsigned int)Voltage0%100/10+0x30);
		Lcd1602_Write_Data((unsigned int)Voltage0%100%10+0x30);	
		Lcd1602_Write_Data('V');

		adc1=ADC0832(1,1);						//通道1
		Voltage1 = (float)adc1 * 5.0/255;
		Voltage1 = Voltage1 *100;				//扩大100倍方便显示

		Lcd1602_Write_Com(0xC0+3);
		Lcd1602_Write_Data((unsigned int)Voltage1/100+0x30);
		Lcd1602_Write_Data('.');
		Lcd1602_Write_Data((unsigned int)Voltage1%100/10+0x30);
		Lcd1602_Write_Data((unsigned int)Voltage1%100%10+0x30);
		Lcd1602_Write_Data('V');
	
	}
}
