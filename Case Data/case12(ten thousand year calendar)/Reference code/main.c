#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char	// 以后unsigned char就可以用uchar代替
#define uint  unsigned int	// 以后unsigned int 就可以用uint 代替

sfr ISP_DATA  = 0xe2;		// 数据寄存器
sfr ISP_ADDRH = 0xe3;		// 地址寄存器高八位
sfr ISP_ADDRL = 0xe4;		// 地址寄存器低八位
sfr ISP_CMD   = 0xe5;		// 命令寄存器
sfr ISP_TRIG  = 0xe6;		// 命令触发寄存器
sfr ISP_CONTR = 0xe7;		// 命令寄存器

sbit LcdRs_P    = P2^7;       // 1602液晶的RS管脚       
sbit LcdRw_P    = P2^6;       // 1602液晶的RW管脚 
sbit LcdEn_P    = P2^5;       // 1602液晶的EN管脚
sbit RST_P      = P1^3;				// 时钟芯片DS1302的RST管脚
sbit SDA_P      = P1^2;				// 时钟芯片DS1302的SDA管脚
sbit SCK_P      = P1^1;				// 时钟芯片DS1302的SCK管脚
sbit KeySet_P   = P3^2;				// 设置时间按键
sbit KeyClock_P = P3^3;				// 设置闹钟按键
sbit KeyDown_P  = P3^4;				// 减按键
sbit KeyUp_P    = P3^5;				// 加按键
sbit Buzzer_P   = P2^0;				// 蜂鸣器


uchar TimeBuff[7]={16,8,5,6,18,30,50};				// 时间数组，默认2016年8月5日，星期五，16:30:50
// TimeBuff[0] 代表年份，范围00-99
// TimeBuff[1] 代表月份，范围1-12
// TimeBuff[2] 代表日期，范围1-31
// TimeBuff[3] 代表星期，范围1-7，1是星期天，2是星期一... ...
// TimeBuff[4] 代表小时，范围00-23
// TimeBuff[5] 代表分钟，范围00-59
// TimeBuff[6] 代表秒钟，范围00-59


uchar Clock_Hour;					// 闹钟的小时
uchar Clock_Minute;				// 闹钟的分钟
uchar Clock_Swt;					// 闹钟的开关
uchar Buzzer_Flag=0;			// 蜂鸣器工作标志



/*********************************************************/
// 单片机内部EEPROM不使能
/*********************************************************/
void ISP_Disable()
{
	ISP_CONTR = 0;
	ISP_ADDRH = 0;
	ISP_ADDRL = 0;
}


/*********************************************************/
// 从单片机内部EEPROM读一个字节，从0x2000地址开始
/*********************************************************/
unsigned char EEPROM_Read(unsigned int add)
{
	ISP_DATA  = 0x00;
	ISP_CONTR = 0x83;
	ISP_CMD   = 0x01;
	ISP_ADDRH = (unsigned char)(add>>8);
	ISP_ADDRL = (unsigned char)(add&0xff);
	// 对STC89C51系列来说，每次要写入0x46，再写入0xB9,ISP/IAP才会生效
	ISP_TRIG  = 0x46;	   
	ISP_TRIG  = 0xB9;
	_nop_();
	ISP_Disable();
	return (ISP_DATA);
}


/*********************************************************/
// 往单片机内部EEPROM写一个字节，从0x2000地址开始
/*********************************************************/
void EEPROM_Write(unsigned int add,unsigned char ch)
{
	ISP_CONTR = 0x83;
	ISP_CMD   = 0x02;
	ISP_ADDRH = (unsigned char)(add>>8);
	ISP_ADDRL = (unsigned char)(add&0xff);
	ISP_DATA  = ch;
	ISP_TRIG  = 0x46;
	ISP_TRIG  = 0xB9;
	_nop_();
	ISP_Disable();
}


/*********************************************************/
// 擦除单片机内部EEPROM的一个扇区
// 写8个扇区中随便一个的地址，便擦除该扇区，写入前要先擦除
/*********************************************************/
void Sector_Erase(unsigned int add)	  
{
	ISP_CONTR = 0x83;
	ISP_CMD   = 0x03;
	ISP_ADDRH = (unsigned char)(add>>8);
	ISP_ADDRL = (unsigned char)(add&0xff);
	ISP_TRIG  = 0x46;
	ISP_TRIG  = 0xB9;
	_nop_();
	ISP_Disable();
}


/*********************************************************/
// 毫秒级的延时函数，time是要延时的毫秒数
/*********************************************************/
void DelayMs(uint time)
{
	uint i,j;
	for(i=time;i>0;i--)
		for(j=112;j>0;j--);
}


/*********************************************************/
// 1602液晶写命令函数，cmd就是要写入的命令
/*********************************************************/
void LcdWriteCmd(uchar cmd)
{ 
	LcdRs_P = 0;
	LcdRw_P = 0;
	LcdEn_P = 0;
	P0=cmd;
	DelayMs(2);
	LcdEn_P = 1;    
	DelayMs(2);
	LcdEn_P = 0;	
}


/*********************************************************/
// 1602液晶写数据函数，dat就是要写入的命令
/*********************************************************/
void LcdWriteData(uchar dat)
{
	LcdRs_P = 1; 
	LcdRw_P = 0;
	LcdEn_P = 0;
	P0=dat;
	DelayMs(2);
	LcdEn_P = 1;    
	DelayMs(2);
	LcdEn_P = 0;
}


/*********************************************************/
// 1602液晶初始化函数
/*********************************************************/
void LcdInit()
{
	LcdWriteCmd(0x38);        // 16*2显示，5*7点阵，8位数据口
	LcdWriteCmd(0x0C);        // 开显示，不显示光标
	LcdWriteCmd(0x06);        // 地址加1，当写入数据后光标右移
	LcdWriteCmd(0x01);        // 清屏
}


/*********************************************************/
// 液晶光标定位函数
/*********************************************************/
void LcdGotoXY(uchar line,uchar column)
{
	// 第一行
	if(line==0)        
		LcdWriteCmd(0x80+column); 
	// 第二行
	if(line==1)        
		LcdWriteCmd(0x80+0x40+column); 
}


/*********************************************************/
// 液晶输出字符串函数
/*********************************************************/
void LcdPrintStr(uchar *str)
{
	while(*str!='\0')
			LcdWriteData(*str++);
}


/*********************************************************/
// 液晶显示内容的初始化
/*********************************************************/
void LcdShowInit()
{
	LcdGotoXY(0,0);
	LcdPrintStr("20  -  -        ");
	LcdGotoXY(1,0);
	LcdPrintStr("      :  :      ");
}



/*********************************************************/
// 液晶输出数字
/*********************************************************/
void LcdPrintNum(uchar num)
{
	LcdWriteData(num/10+48);		// 十位
	LcdWriteData(num%10+48); 		// 个位
}


/*********************************************************/
// 液晶显示星期
/*********************************************************/
void LcdPrintWeek(uchar week)
{
	switch(week)
	{
		case 1: LcdPrintStr(" Sun");	break;
		case 2: LcdPrintStr(" Mon");	break;
		case 3: LcdPrintStr("Tues");	break;
		case 4: LcdPrintStr(" Wed");	break;
		case 5: LcdPrintStr("Thur");	break;
		case 6: LcdPrintStr(" Fri");	break;
		case 7: LcdPrintStr(" Sat");	break;
		default:											break;
	}
}


/*********************************************************/
// 刷新时间显示
/*********************************************************/
void FlashTime()
{
	LcdGotoXY(0,2);										// 年份
	LcdPrintNum(TimeBuff[0]);
	LcdGotoXY(0,5);										// 月份
	LcdPrintNum(TimeBuff[1]);
	LcdGotoXY(0,8);										// 日期
	LcdPrintNum(TimeBuff[2]);
	LcdGotoXY(1,4);										// 小时
	LcdPrintNum(TimeBuff[4]);
	LcdGotoXY(1,7);										// 分钟
	LcdPrintNum(TimeBuff[5]);
	LcdGotoXY(1,10);									// 秒钟
	LcdPrintNum(TimeBuff[6]);
	LcdGotoXY(0,12);									// 星期	
	LcdPrintWeek(TimeBuff[3]);
}




/*********************************************************/
// 初始化DS1302
/*********************************************************/
void DS1302_Init(void)
{
	RST_P=0;			// RST脚置低
	SCK_P=0;			// SCK脚置低
	SDA_P=0;			// SDA脚置低				
}



/*********************************************************/
// 从DS1302读出一字节数据
/*********************************************************/
uchar DS1302_Read_Byte(uchar addr) 
{
	uchar i;
	uchar temp;
	
	RST_P=1;								
	
	/* 写入目标地址：addr*/
	for(i=0;i<8;i++) 
	{     
		if(addr&0x01) 
			SDA_P=1;
		else 
			SDA_P=0;
		
		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
		
		addr=addr>> 1;
	}
	
	/* 读出该地址的数据 */
	for(i=0;i<8;i++) 
	{
		temp=temp>>1;
		
		if(SDA_P) 
			temp|= 0x80;
		else 
			temp&=0x7F;
		
		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
	}
	
	RST_P=0;
	
	return temp;
}



/*********************************************************/
// 向DS1302写入一字节数据
/*********************************************************/
void DS1302_Write_Byte(uchar addr, uchar dat)
{
	uchar i;
	
	RST_P = 1;
	
	/* 写入目标地址：addr*/
	for(i=0;i<8;i++) 
	{ 
		if(addr&0x01) 
			SDA_P=1;
		else 
			SDA_P=0;

		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
		
		addr=addr>>1;
	}
	
	/* 写入数据：dat*/
	for(i=0;i<8;i++) 
	{
		if(dat&0x01) 
			SDA_P=1;
		else 
			SDA_P=0;
	
		SCK_P=1;
		_nop_();
		SCK_P=0;
		_nop_();
		
		dat=dat>>1;
	}
	
	RST_P=0;					
}



/*********************************************************/
// 向DS1302写入时间数据
/*********************************************************/
void DS1302_Write_Time() 
{
  uchar i;
	uchar temp1;
	uchar temp2;
	
	for(i=0;i<7;i++)			// 十进制转BCD码
	{
		temp1=(TimeBuff[i]/10)<<4;
		temp2=TimeBuff[i]%10;
		TimeBuff[i]=temp1+temp2;
	}
	
	DS1302_Write_Byte(0x8E,0x00);								// 关闭写保护 
	DS1302_Write_Byte(0x80,0x80);								// 暂停时钟 
	DS1302_Write_Byte(0x8C,TimeBuff[0]);				// 年 
	DS1302_Write_Byte(0x88,TimeBuff[1]);				// 月 
	DS1302_Write_Byte(0x86,TimeBuff[2]);				// 日 
	DS1302_Write_Byte(0x8A,TimeBuff[3]);				// 星期
	DS1302_Write_Byte(0x84,TimeBuff[4]);				// 时 
	DS1302_Write_Byte(0x82,TimeBuff[5]);				// 分
	DS1302_Write_Byte(0x80,TimeBuff[6]);				// 秒
	DS1302_Write_Byte(0x80,TimeBuff[6]&0x7F);		// 运行时钟
	DS1302_Write_Byte(0x8E,0x80);								// 打开写保护  
}




/*********************************************************/
// 从DS1302读出时间数据
/*********************************************************/
void DS1302_Read_Time()  
{ 
	uchar i;

	TimeBuff[0]=DS1302_Read_Byte(0x8D);						// 年 
	TimeBuff[1]=DS1302_Read_Byte(0x89);						// 月 
	TimeBuff[2]=DS1302_Read_Byte(0x87);						// 日 
	TimeBuff[3]=DS1302_Read_Byte(0x8B);						// 星期
	TimeBuff[4]=DS1302_Read_Byte(0x85);						// 时 
	TimeBuff[5]=DS1302_Read_Byte(0x83);						// 分 
	TimeBuff[6]=(DS1302_Read_Byte(0x81))&0x7F;		// 秒 

	for(i=0;i<7;i++)		// BCD转十进制
	{           
		TimeBuff[i]=(TimeBuff[i]/16)*10+TimeBuff[i]%16;
	}
}




/*********************************************************/
// 按键扫描(设置时间)
/*********************************************************/
void KeyScanf1()
{
	if(KeySet_P==0)
	{
		LcdWriteCmd(0x0f);			// 启动光标闪烁
		LcdGotoXY(0,3);					// 定位光标到年份闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整年份 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[0]>0)						// 判断年份是否大于0		
					TimeBuff[0]--;						// 是的话就减去1
				LcdGotoXY(0,2);							// 光标定位到年份的位置
				LcdPrintNum(TimeBuff[0]);		// 刷新显示改变后的年份
				LcdGotoXY(0,3);							// 定位光标到年份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[0]<99)					// 判断年份是否小于99
					TimeBuff[0]++;						// 是的话就加上1
				LcdGotoXY(0,2);							// 光标定位到年份的位置
				LcdPrintNum(TimeBuff[0]);		// 刷新显示改变后的年份
				LcdGotoXY(0,3);							// 定位光标到年份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,6);					// 定位光标到月份闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
			
		/* 调整月份 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[1]>1)						// 判断月份是否大于1		
					TimeBuff[1]--;						// 是的话就减去1
				LcdGotoXY(0,5);							// 光标定位到月份的位置
				LcdPrintNum(TimeBuff[1]);		// 刷新显示改变后的月份
				LcdGotoXY(0,6);							// 定位光标到月份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[1]<12)					// 判断月份是否小于12
					TimeBuff[1]++;						// 是的话就加上1
				LcdGotoXY(0,5);							// 光标定位到月份的位置
				LcdPrintNum(TimeBuff[1]);		// 刷新显示改变后的月份
				LcdGotoXY(0,6);							// 定位光标到月份闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,9);					// 定位光标到日期闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整日期 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[2]>1)						// 判断日期是否大于1		
					TimeBuff[2]--;						// 是的话就减去1
				LcdGotoXY(0,8);							// 光标定位到日期的位置
				LcdPrintNum(TimeBuff[2]);		// 刷新显示改变后的日期
				LcdGotoXY(0,9);							// 定位光标到日期闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[2]<31)					// 判断日期是否小于31
					TimeBuff[2]++;						// 是的话就加上1
				LcdGotoXY(0,8);							// 光标定位到日期的位置
				LcdPrintNum(TimeBuff[2]);		// 刷新显示改变后的日期
				LcdGotoXY(0,9);							// 定位光标到日期闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(0,15);				// 定位光标到星期闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整星期 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[3]>1)						// 判断星期是否大于1		
					TimeBuff[3]--;						// 是的话就减去1
				LcdGotoXY(0,12);						// 光标定位到星期的位置
				LcdPrintWeek(TimeBuff[3]);	// 刷新显示改变后的星期
				LcdGotoXY(0,15);						// 定位光标到星期闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[3]<7)						// 判断星期是否小于6
					TimeBuff[3]++;						// 是的话就加上1
				LcdGotoXY(0,12);						// 光标定位到星期的位置
				LcdPrintWeek(TimeBuff[3]);	// 刷新显示改变后的星期
				LcdGotoXY(0,15);						// 定位光标到星期闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,5);					// 定位光标到小时闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整小时 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[4]>0)						// 判断小时是否大于0
					TimeBuff[4]--;						// 是的话就减去1
				LcdGotoXY(1,4);							// 光标定位到小时的位置
				LcdPrintNum(TimeBuff[4]);		// 刷新显示改变后的小时
				LcdGotoXY(1,5);							// 定位光标到小时闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[4]<23)					// 判断小时是否小于23
					TimeBuff[4]++;						// 是的话就加上1
				LcdGotoXY(1,4);							// 光标定位到小时的位置
				LcdPrintNum(TimeBuff[4]);		// 刷新显示改变后的小时
				LcdGotoXY(1,5);							// 定位光标到小时闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,8);					// 定位光标到分钟闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整分钟 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[5]>0)						// 判断分钟是否大于0
					TimeBuff[5]--;						// 是的话就减去1
				LcdGotoXY(1,7);							// 光标定位到分钟的位置
				LcdPrintNum(TimeBuff[5]);		// 刷新显示改变后的分钟
				LcdGotoXY(1,8);							// 定位光标到分钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[5]<59)					// 判断分钟是否小于59
					TimeBuff[5]++;						// 是的话就加上1
				LcdGotoXY(1,7);							// 光标定位到分钟的位置
				LcdPrintNum(TimeBuff[5]);		// 刷新显示改变后的分钟
				LcdGotoXY(1,8);							// 定位光标到分钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,11);				// 定位光标到秒钟闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整秒钟 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(TimeBuff[6]>0)						// 判断秒钟是否大于0
					TimeBuff[6]--;						// 是的话就减去1
				LcdGotoXY(1,10);						// 光标定位到秒钟的位置
				LcdPrintNum(TimeBuff[6]);		// 刷新显示改变后的秒钟
				LcdGotoXY(1,11);						// 定位光标到秒钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(TimeBuff[6]<59)					// 判断秒钟是否小于59
					TimeBuff[6]++;						// 是的话就加上1
				LcdGotoXY(1,10);						// 光标定位到秒钟的位置
				LcdPrintNum(TimeBuff[6]);		// 刷新显示改变后的秒钟
				LcdGotoXY(1,11);						// 定位光标到秒钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeySet_P==0)
			{
				break;
			}
		}
		
		/* 退出前的设置 */
		LcdWriteCmd(0x0C);			// 关闭光标闪烁
		DS1302_Write_Time();		// 把新设置的时间值存入DS1302芯片
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeySet_P);				// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
	}
}



/*********************************************************/
// 按键扫描(设置闹钟)
/*********************************************************/
void KeyScanf2()
{
	if(KeyClock_P==0)
	{
		LcdGotoXY(0,0);										// 液晶显示为闹钟设置的界面
		LcdPrintStr("Alarm Clock Set ");
		LcdGotoXY(1,0);				
		LcdPrintStr("     :          ");
		LcdGotoXY(1,3);										// 显示闹钟的小时
		LcdPrintNum(Clock_Hour);
		LcdGotoXY(1,6);										// 显示闹钟的分钟
		LcdPrintNum(Clock_Minute);
		LcdGotoXY(1,10);									// 显示闹钟状态
		if(Clock_Swt==0)
		{
			LcdPrintStr("OFF");
		}
		else
		{
			LcdPrintStr(" ON");
		}
		LcdGotoXY(1,4);										// 光标定位
		LcdWriteCmd(0x0f);								// 光标闪烁
		DelayMs(10);											// 延时等待，消除按键按下的抖动
		while(!KeyClock_P);								// 等待按键释放
		DelayMs(10);											// 延时等待，消除按键松开的抖动
		
		/* 调整闹钟小时 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(Clock_Hour>0)						// 判断闹钟小时是否大于0
					Clock_Hour--;							// 是的话就减去1
				LcdGotoXY(1,3);							// 光标定位到闹钟小时的位置
				LcdPrintNum(Clock_Hour);		// 刷新显示改变后的闹钟小时
				LcdGotoXY(1,4);							// 定位光标到闹钟小时闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(Clock_Hour<23)						// 判断闹钟小时是否小于23
					Clock_Hour++;							// 是的话就加上1
				LcdGotoXY(1,3);							// 光标定位到闹钟小时的位置
				LcdPrintNum(Clock_Hour);		// 刷新显示改变后的闹钟小时
				LcdGotoXY(1,4);							// 定位光标到闹钟小时闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyClock_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,7);					// 定位光标到闹钟分钟的闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeyClock_P);			// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 调整分钟 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(Clock_Minute>0)					// 判断闹钟分钟是否大于0
					Clock_Minute--;						// 是的话就减去1
				LcdGotoXY(1,6);							// 光标定位到闹钟分钟的位置
				LcdPrintNum(Clock_Minute);	// 刷新显示改变后的闹钟分钟
				LcdGotoXY(1,7);							// 定位光标到闹钟分钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)								// 如果加按键被下去
			{
				if(Clock_Minute<59)					// 判断闹钟分钟是否小于59
					Clock_Minute++;						// 是的话就加上1
				LcdGotoXY(1,6);							// 光标定位到闹钟分钟的位置
				LcdPrintNum(Clock_Minute);	// 刷新显示改变后的闹钟分钟
				LcdGotoXY(1,7);							// 定位光标到闹钟分钟闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyClock_P==0)
			{
				break;
			}
		}
		
		LcdGotoXY(1,12);				// 定位光标到闹钟开关的位置闪烁
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeyClock_P);			// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		
		/* 闹钟开关 */
		while(1)
		{
			if(KeyDown_P==0)							// 如果减按键被下去
			{
				if(Clock_Swt==1)						// 判断闹钟是否开启
					Clock_Swt=0;							// 关闭闹钟
				LcdGotoXY(1,10);						// 光标定位到秒钟开关的位置
				LcdPrintStr("OFF");					// 液晶显示“OFF”
				LcdGotoXY(1,12);						// 定位光标到闹钟开关的位置闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyUp_P==0)									// 如果加按键被下去
			{
				if(Clock_Swt==0)						// 判断闹钟是否关闭
					Clock_Swt=1;							// 启动闹钟
				LcdGotoXY(1,10);						// 光标定位到秒钟开关的位置
				LcdPrintStr(" ON");					// 液晶显示“ ON”
				LcdGotoXY(1,12);						// 定位光标到闹钟开关的位置闪烁
				DelayMs(300);								// 延时0.3秒左右
			}
			
			if(KeyClock_P==0)
			{
				break;
			}
		}
		
		/* 退出前的设置 */
		LcdWriteCmd(0x0C);			// 关闭光标闪烁
		LcdShowInit();					// 液晶显示内容恢复为检测界面的
		DelayMs(10);						// 延时等待，消除按键按下的抖动
		while(!KeyClock_P);			// 等待按键释放
		DelayMs(10);						// 延时等待，消除按键松开的抖动
		Sector_Erase(0x2000);
		EEPROM_Write(0x2000,Clock_Hour);			// 往0x2000这个地址写入闹钟的小时 
		EEPROM_Write(0x2001,Clock_Minute);		// 往0x2001这个地址写入闹钟的分钟
		EEPROM_Write(0x2002,Clock_Swt);				// 往0x2002这个地址写入闹钟的开关
	}
}



/*********************************************************/
// 闹钟判断
/*********************************************************/
void ClockJudge()
{
	if(Clock_Swt==1)			// 判断闹钟的开关是否开启
	{
		if((Clock_Hour==TimeBuff[4])&&(Clock_Minute==TimeBuff[5]))		// 当前小时和分钟，和闹钟的小时和分钟是否一致
		{
			if(TimeBuff[6]==0)								// 秒数是否等于0
			{
				Buzzer_Flag=1;									// 开启蜂鸣器报警标志
			}
		}
	}
	
	if(TimeBuff[6]==59)										// 如果当前秒数为59秒
	{
		Buzzer_Flag=0;											// 关闭蜂鸣器报警标志
	}
	
	if((KeyDown_P==0)||(KeyUp_P==0))			// 如果加按键或减按键被按下
	{
		Buzzer_Flag=0;											// 关闭蜂鸣器报警标志
	}
	
	if(Buzzer_Flag==1)										// 如果蜂鸣器报警标志为启动
	{
		Buzzer_P=0;													// 启动蜂鸣器
		DelayMs(100);												// 延时0.1秒
		Buzzer_P=1;													// 关闭蜂鸣器
		DelayMs(100);												// 延时0.1秒
	}
}


/*********************************************************/
// 主函数
/*********************************************************/
void main()
{
	LcdInit();			// 执行液晶初始化	
	DS1302_Init();	// 时钟芯片的初始化
	LcdShowInit();	// 液晶显示内容的初始化

	if(DS1302_Read_Byte(0x81)>=128)			// 判断时钟芯片是否正在运行
	{
		DS1302_Write_Time();							// 如果没有，则初始化一个时间
	}
	

	Clock_Hour=EEPROM_Read(0x2000);				// 读取0x2000这个地址的内容，赋值给闹钟的小时变量
	if(Clock_Hour>23)											// 如果读取到的闹钟小时数值不正常，则重新赋值
	{
		Clock_Hour=12;
	}
  Clock_Minute=EEPROM_Read(0x2001);			// 读取0x2001这个地址的内容，赋值给闹钟的分钟变量
	if(Clock_Minute>59)										// 如果读取到的闹钟分钟数值不正常，则重新赋值
	{
		Clock_Minute=30;
	}
	Clock_Swt=EEPROM_Read(0x2002);				// 读取0x2002这个地址的内容，赋值给闹钟的开关变量
	if(Clock_Swt>1)												// 如果读取到的闹钟开关数值不正常，则重新赋值
	{
		Clock_Swt=0;
	}
	
	
	while(1)
	{
		DS1302_Read_Time();				// 获取当前时钟芯片的时间，存在数组time_buf中
		
		ClockJudge();							// 闹钟工作的判断
		
		FlashTime();							// 刷新时间显示

		KeyScanf1();							// 按键扫描(时间的设置)
		
		KeyScanf2();							// 按键扫描(闹钟的设置)
		
		DelayMs(100);							// 延时0.1秒
	}
}




