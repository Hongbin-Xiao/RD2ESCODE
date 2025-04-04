/***********************************************
�������ƣ�ģ��ת����ADC0832 A/Dת�����̣�29������51��Ƭ�������ֵ�ѹ��ADC0832����
����˵����Һ��1602��ʾADC0832 ��·A/Dת����� ��ʾ��ѹֵ 0-5V��Χ
�ӿ�˵����RS     P2^5;	
		  RW  	 P2^6;
		  E  	 P2^7;	
		  ���ݿ� P0 
		  ADCLK	 P1^0;	  
		  ADDO   P1^1;
		  ADDI   P1^1;
		  ADCS   P1^3;
���뻷����keil4
MCU     ��STC89C52RC
ʱ��Ƶ��: 11.0592MHz
����    ����΢���ӹ�����
�Ա����̣�https://shop129920945.taobao.com
***********************************************/
#include "reg52.h"
#include "intrins.h"

	
#define LcdDataPort P0   //����λ�궨��

sbit RS=P2^5;	 	 	 //��������ѡ���
sbit RW=P2^6;            //��дѡ���
sbit EN=P2^7;            //ʹ���źŶ�
/************************************************************************
* ����: void delay_ms(unsigned int nms)	
* ����: ��ʱ��������ʱnms����   11.0592MHZ
* ����: nms :  ����
* ����: none.
************************************************************************/
void delay_ms(unsigned int nms)		
{
	unsigned int i ,j;
	for(i=nms;i>0;i--)              //nms����ʱԼn���� 
	{	for(j=110;j>0;j--);	}
}
/************************************************************************
* ����: void delay_10us(unsigned int nus) 
* ����: ��ʱ��������ʱnus*10΢��    �Ǿ�׼��ʱ 
* ����: nus :  ΢��
* ����: none.
************************************************************************/
void delay_10us(unsigned int nus) 
{
	while(nus--);
}

/******************************************************************************

 ******************************************************************************
*  �� �� ��   : LCD1602.c
lcd1602�ӳ���
******************************************************************************/
/************************************************************************
* ����: bit Lcd1602_Check_Busy(void)
* ����: ��æ����
* ����: none.
* ����: 1 ��æ
		0 ����æ
************************************************************************/    
bit Lcd1602_Check_Busy(void)      		//��æ����
{  
   	register unsigned char busy;
	RS=0;				  				//��״̬ʱ RS = 0 RW = 1..EN = 1;
	RW=1;
	EN=1;
	delay_10us(30);
	busy=LcdDataPort;
	EN=0;
	return(busy&0x80);	 
}
/************************************************************************
* ����: void Lcd1602_Lcd1602_Write_Com(unsigned char com)
* ����: Һ��д����
* ����: com : Ҫд������
* ����: none.
************************************************************************/
void Lcd1602_Write_Com(unsigned char com)	
{
    while(Lcd1602_Check_Busy());		//æ��ȴ�
	RS=0;								//дָ��ʱ RS = 0 RW = 0 ..EN 1->0		  
	RW=0;
	EN=1;	
	LcdDataPort=com;
	EN=0;
} 
/************************************************************************
* ����: void Lcd1602_Write_Data(unsigned char dat)
* ����: Һ��д����  Ҫ��ʾʲô�ַ���д��
* ����: dat ��Ҫд������
* ����: none. 
************************************************************************/
void Lcd1602_Write_Data(unsigned char dat)	
{
	while(Lcd1602_Check_Busy());		//æ��ȴ�
	RS=1;					   			//д����ʱ RS = 1 RW = 0 ..EN 1->0
	RW=0;
	EN=1;	
	LcdDataPort=dat;
	EN=0; 
}
/************************************************************************
* ����: void write_string(unsigned char x,unsigned char y,unsigned char *string)
* ����: ��ʾ�ַ�������
* ����: x : x����
		y : y����
		*string : Ҫ��ʾ���ַ���
* ����: none.
************************************************************************/
void Lcd1602_String(unsigned char x,unsigned char y,unsigned char *string)	
{
	if(y==0)                  			 //������
		Lcd1602_Write_Com(0x80+x);       //��һ��0x80��ͷ
	else
		Lcd1602_Write_Com(0xc0+x);		 //�ڶ���0xc0��ͷ
	while(*string)                       //�ַ�������ַ
	{
		Lcd1602_Write_Data(*string);
		string++;
	}
}
/************************************************************************
* ����: void Lcd_Init(void)	
* ����: LCD1602Һ����ʼ��
* ����: none.
* ����: none. 
************************************************************************/							 
void Lcd1602_Init(void)		  //LCD1602Һ����ʼ��
{
	 Lcd1602_Write_Com(0x38); //������ʾģʽ 16*2��ʾ 5*7���� 8λ���ݽӿ�
	 Lcd1602_Write_Com(0x0c); //��ʾ����������ʾ��ꡢ��겻��ʾ
	 Lcd1602_Write_Com(0x06); //д��һ���ַ����ַ�Զ���1����Ļ���ƶ�
	 Lcd1602_Write_Com(0x01); //������긴λ
}




sbit ADCLK =P1^0;	//AD0832ʱ�ӽӿ�
sbit ADDI = P1^1;	//AD0832�����ź����룬ѡ��ͨ������
sbit ADDO = P1^1;	//AD0832�����ź������ת���������
sbit ADCS = P1^3;	//AD0832Ƭѡ�ӿ�


/************************************************************************
* ����: unsigned char ADC0832(bit mode,bit channel)
* ����: ADC0832 ת������
* ����: mode�� ���͵�ƽΪ���ģʽ���ߵ�ƽΪ��ͨ��ģʽ��
		channel��ͨ������0��1������
* ����: 0��ת��ʧ�ܡ���������ΪA/Dת����ֵ 
* ��ע: A/Dת����ֵ��ΧΪ0-256   8λA/D  ��2��8�η�=256
************************************************************************/    
unsigned char ADC0832(bit mode,bit channel) //ADת�������ؽ��
{
	unsigned char i,dat,ndat;
	ADCS = 0;//����CS��
	_nop_();
	_nop_();
	ADDI = 1; //��1���½���Ϊ�ߵ�ƽ
	ADCLK = 1;//����CLK��
	_nop_();
	_nop_();
	ADCLK = 0;//����CLK��,�γ��½���1
	_nop_();
	_nop_();
	ADDI = mode; //�͵�ƽΪ���ģʽ���ߵ�ƽΪ��ͨ��ģʽ��
	ADCLK = 1;//����CLK��
	_nop_();
	_nop_();
	ADCLK = 0;//����CLK��,�γ��½���2
	_nop_();
	_nop_();
	ADDI = channel; //�͵�ƽΪCH0���ߵ�ƽΪCH1
	ADCLK = 1;//����CLK��
	_nop_();
	_nop_();
	ADCLK = 0;//����CLK��,�γ��½���3
	ADDI = 1;//�����������(���������)
	dat = 0;
	//���濪ʼ��ȡת��������ݣ������λ��ʼ���������D7~D0�� 
	for(i = 0;i < 8;i++)
	{
		dat <<= 1;
		ADCLK=1;//����ʱ�Ӷ�
		_nop_();
		_nop_();
		ADCLK=0;//����ʱ�Ӷ��γ�һ��ʱ������
		_nop_();
		_nop_();
		dat |= ADDO;
	}
	ndat = 0; //��¼D0
	if(ADDO == 1)
		ndat |= 0x80;
	//���濪ʼ������ȡ��������ݣ���D1��D7��
	for(i = 0;i < 7;i++)
	{
		ndat >>= 1;
		ADCLK = 1;//����ʱ�Ӷ�
		_nop_();
		_nop_();
		ADCLK=0;//����ʱ�Ӷ��γ�һ��ʱ������
		_nop_();
		_nop_();
		if(ADDO==1)
		ndat |= 0x80;
	}
	ADCS=1;//����CS��,����ת��
	ADCLK=0;//����CLK��
	ADDI=1;//�������ݶ�,�ص���ʼ״̬
	if(dat==ndat)
		return(dat);
	else
		return 0;
}



	
void main(void)
{
	unsigned char adc0,adc1;
	unsigned char num=0;
	unsigned char ADC_Get[10]={0};  		//����AD��������

	float Voltage0 = 0.0;
	float Voltage1 = 0.0;

	Lcd1602_Init();			  				//LCD1602Һ����ʼ��

	Lcd1602_String(0,0,"V0:         ");
	Lcd1602_String(0,1,"V1:         ");
	while(1)
	{

		adc0=ADC0832(1,0);			//ͨ��0  
		Voltage0 = (float)adc0 * 5.0/255;
		Voltage0 = Voltage0 *100;				//����100��������ʾ
		Lcd1602_Write_Com(0x80+3);
		Lcd1602_Write_Data((unsigned int)Voltage0/100+0x30);
		Lcd1602_Write_Data('.');
		Lcd1602_Write_Data((unsigned int)Voltage0%100/10+0x30);
		Lcd1602_Write_Data((unsigned int)Voltage0%100%10+0x30);	
		Lcd1602_Write_Data('V');

		adc1=ADC0832(1,1);						//ͨ��1
		Voltage1 = (float)adc1 * 5.0/255;
		Voltage1 = Voltage1 *100;				//����100��������ʾ

		Lcd1602_Write_Com(0xC0+3);
		Lcd1602_Write_Data((unsigned int)Voltage1/100+0x30);
		Lcd1602_Write_Data('.');
		Lcd1602_Write_Data((unsigned int)Voltage1%100/10+0x30);
		Lcd1602_Write_Data((unsigned int)Voltage1%100%10+0x30);
		Lcd1602_Write_Data('V');
	
	}
}
