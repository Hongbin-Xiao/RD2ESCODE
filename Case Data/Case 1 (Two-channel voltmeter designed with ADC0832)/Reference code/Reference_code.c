/***********************************************
Code Name: ADC0832 A/D Converter Example (29, 51 MCU-based Digital Voltmeter (ADC0832))
Function: LCD1602 displays ADC0832 dual-channel A/D conversion results, voltage range 0-5V
Interface: RS     P2^5;	
          RW     P2^6;
          E      P2^7;	
          Data   P0 
          ADCLK  P1^0;	  
          ADDO   P1^1;
          ADDI   P1^1;
          ADCS   P1^3;
Compiler: keil4
MCU     : STC89C52RC
Clock   : 11.0592MHz
Author  : Kang Wei Electronics Studio
Shop    : https://shop129920945.taobao.com
***********************************************/
#include "reg52.h"
#include "intrins.h"

	
#define LcdDataPort P0   //Data port macro definition

sbit RS=P2^5;	 	 	 //Data command selection pin
sbit RW=P2^6;            //Read/write selection pin
sbit EN=P2^7;            //Enable signal pin
/************************************************************************
* Function: void delay_ms(unsigned int nms)	
* Description: Delay function, delay nms milliseconds   11.0592MHZ
* Parameter: nms : milliseconds
* Return: none.
************************************************************************/
void delay_ms(unsigned int nms)		
{
	unsigned int i ,j;
	for(i=nms;i>0;i--)              //nms means delay about n milliseconds 
	{	for(j=110;j>0;j--);	}
}
/************************************************************************
* Function: void delay_10us(unsigned int nus) 
* Description: Delay function, delay nus*10 microseconds    Non-precise delay 
* Parameter: nus : microseconds
* Return: none.
************************************************************************/
void delay_10us(unsigned int nus) 
{
	while(nus--);
}

/******************************************************************************

 ******************************************************************************
*  File Name   : LCD1602.c
LCD1602 subroutines
******************************************************************************/
/************************************************************************
* Function: bit Lcd1602_Check_Busy(void)
* Description: Check busy function
* Parameter: none.
* Return: 1 : Busy
         0 : Not busy
************************************************************************/    
bit Lcd1602_Check_Busy(void)      		//Check busy function
{  
   	register unsigned char busy;
	RS=0;				  				//When reading status RS = 0 RW = 1..EN = 1;
	RW=1;
	EN=1;
	delay_10us(30);
	busy=LcdDataPort;
	EN=0;
	return(busy&0x80);	 
}
/************************************************************************
* Function: void Lcd1602_Lcd1602_Write_Com(unsigned char com)
* Description: LCD write command
* Parameter: com : Command to write
* Return: none.
************************************************************************/
void Lcd1602_Write_Com(unsigned char com)	
{
    while(Lcd1602_Check_Busy());		//Wait if busy
	RS=0;								//When writing instruction RS = 0 RW = 0 ..EN 1->0		  
	RW=0;
	EN=1;	
	LcdDataPort=com;
	EN=0;
} 
/************************************************************************
* Function: void Lcd1602_Write_Data(unsigned char dat)
* Description: LCD write data - write the character to display
* Parameter: dat : Data to write
* Return: none. 
************************************************************************/
void Lcd1602_Write_Data(unsigned char dat)	
{
	while(Lcd1602_Check_Busy());		//Wait if busy
	RS=1;					   			//When writing data RS = 1 RW = 0 ..EN 1->0
	RW=0;
	EN=1;	
	LcdDataPort=dat;
	EN=0; 
}
/************************************************************************
* Function: void write_string(unsigned char x,unsigned char y,unsigned char *string)
* Description: Display string function
* Parameter: x : x coordinate
            y : y coordinate
            *string : String to display
* Return: none.
************************************************************************/
void Lcd1602_String(unsigned char x,unsigned char y,unsigned char *string)	
{
	if(y==0)                  			 //Set row
		Lcd1602_Write_Com(0x80+x);       //First row starts with 0x80
	else
		Lcd1602_Write_Com(0xc0+x);		 //Second row starts with 0xc0
	while(*string)                       //String address
	{
		Lcd1602_Write_Data(*string);
		string++;
	}
}
/************************************************************************
* Function: void Lcd_Init(void)	
* Description: LCD1602 initialization
* Parameter: none.
* Return: none. 
************************************************************************/							 
void Lcd1602_Init(void)		  //LCD1602 initialization
{
	 Lcd1602_Write_Com(0x38); //Set display mode 16*2 display 5*7 dot matrix 8-bit data interface
	 Lcd1602_Write_Com(0x0c); //Display on, cursor off, cursor not displayed
	 Lcd1602_Write_Com(0x06); //After writing a character, address auto-increments, screen doesn't move
	 Lcd1602_Write_Com(0x01); //Clear screen, cursor reset
}




sbit ADCLK =P1^0;	//AD0832 clock interface
sbit ADDI = P1^1;	//AD0832 data signal input, channel selection control
sbit ADDO = P1^1;	//AD0832 data signal output, conversion data output
sbit ADCS = P1^3;	//AD0832 chip select interface
 
unsigned char ADC0832(bit mode,bit channel) //
{
	unsigned char i,dat,ndat;
	ADCS = 0;//Pull CS low
	_nop_();
	_nop_();
	ADDI = 1; //First falling edge is high level
	ADCLK = 1;//Pull CLK high
	_nop_();
	_nop_();
	ADCLK = 0;//Pull CLK low, forming falling edge 1
	_nop_();
	_nop_();
	ADDI = mode; //Low level for differential mode, high level for single channel mode.
	ADCLK = 1;//Pull CLK high
	_nop_();
	_nop_();
	ADCLK = 0;//Pull CLK low, forming falling edge 2
	_nop_();
	_nop_();
	ADDI = channel; //Low level for CH0, high level for CH1
	ADCLK = 1;//Pull CLK high
	_nop_();
	_nop_();
	ADCLK = 0;//Pull CLK low, forming falling edge 3
	ADDI = 1;//Control command ends (necessary from testing)
	dat = 0;
	//Now start reading the converted data, output from MSB to LSB (D7~D0) 
	for(i = 0;i < 8;i++)
	{
		dat <<= 1;
		ADCLK=1;//Pull clock high
		_nop_();
		_nop_();
		ADCLK=0;//Pull clock low to form one clock pulse
		_nop_();
		_nop_();
		dat |= ADDO;
	}
	ndat = 0; //Record D0
	if(ADDO == 1)
		ndat |= 0x80;
	//Now continue reading the reversed data (from D1 to D7)
	for(i = 0;i < 7;i++)
	{
		ndat >>= 1;
		ADCLK = 1;//Pull clock high
		_nop_();
		_nop_();
		ADCLK=0;//Pull clock low to form one clock pulse
		_nop_();
		_nop_();
		if(ADDO==1)
		ndat |= 0x80;
	}
	ADCS=1;//Pull CS high, end conversion
	ADCLK=0;//Pull CLK low
	ADDI=1;//Pull data high, return to initial state
	if(dat==ndat)
		return(dat);
	else
		return 0;
}



	
void main(void)
{
	unsigned char adc0,adc1;
	unsigned char num=0;
	unsigned char ADC_Get[10]={0};  		//Define AD sampling array

	float Voltage0 = 0.0;
	float Voltage1 = 0.0;

	Lcd1602_Init();			  				//LCD1602 initialization

	Lcd1602_String(0,0,"V0:         ");
	Lcd1602_String(0,1,"V1:         ");
	while(1)
	{

		adc0=ADC0832(1,0);			//Channel 0  
		Voltage0 = (float)adc0 * 5.0/255;
		Voltage0 = Voltage0 *100;				//Multiply by 100 for easier display
		Lcd1602_Write_Com(0x80+3);
		Lcd1602_Write_Data((unsigned int)Voltage0/100+0x30);
		Lcd1602_Write_Data('.');
		Lcd1602_Write_Data((unsigned int)Voltage0%100/10+0x30);
		Lcd1602_Write_Data((unsigned int)Voltage0%100%10+0x30);	
		Lcd1602_Write_Data('V');

		adc1=ADC0832(1,1);						//Channel 1
		Voltage1 = (float)adc1 * 5.0/255;
		Voltage1 = Voltage1 *100;				//Multiply by 100 for easier display

		Lcd1602_Write_Com(0xC0+3);
		Lcd1602_Write_Data((unsigned int)Voltage1/100+0x30);
		Lcd1602_Write_Data('.');
		Lcd1602_Write_Data((unsigned int)Voltage1%100/10+0x30);
		Lcd1602_Write_Data((unsigned int)Voltage1%100%10+0x30);
		Lcd1602_Write_Data('V');
	
	}
}
