#include<AT89X52.h>
#include <intrins.h>

#define Delaynop(){_nop_();_nop_();_nop_();_nop_();}


sbit RS=P2^0;
sbit RW=P2^1;
sbit EN=P2^2;

void Delayms(unsigned int ms);
bit LCD_Busy();
void LCD_Pos(unsigned char);
void LCD_Wcmd(unsigned char);
void LCD_Wdat(unsigned char);

char code Title_Text[]={"--Phone Code--  "};

unsigned char code Key_Table[]={'1','2','3','4','5','6','7','8','9','*','0','#'};
unsigned char Dial_Code_Str[]={"             "};
unsigned char KeyNo=0xff;
int tCount=0;
//毫秒级延时函数
void Delayms(unsigned int x)
{
  unsigned char i;
  while(x--)
  { 
    for(i=0;i<120;i++);
  }
}
//显示字符串
void Display_String(unsigned char *str,unsigned char LineNo)
{
  unsigned char k;
  LCD_Pos(LineNo);
  for(k=0;k<16;k++)
  {
    LCD_Wdat(str[k]);
  }
}
//检查1602是否处于忙装填
bit LCD_Busy_Check()
{
  bit Result;
  RS=0;
  RW=1;
  EN=1;
  Delaynop();
  Result=(bit)(P0 & 0x80);
  EN=0;
  return Result;
}
//向1602写入指令的函数
void LCD_Write_Command(unsigned char cmd)	
{
  while(LCD_Busy_Check());  //检查是否处于忙状态
  RS=0;                 
  RW=0;
  EN=0; 
  _nop_();
  _nop_();
  P0=cmd;   //写入指令
  Delaynop();
  EN=1;
  Delaynop();
  EN=0;
 }
 //向1602写数据
void LCD_Wdat(unsigned char dat)
{
 while(LCD_Busy_Check());  //检查是否处于忙状态
 RS=1;
 RW=0;
 EN=0;
 P0=dat;                   //写入数据
 Delaynop();
 EN=1;
 Delaynop();
 EN=0;
}
//初始化1602
void Init_LCD()
{
  LCD_Write_Command(0x38);Delayms(5);
  LCD_Write_Command(0x01);Delayms(5);
  LCD_Write_Command(0x06);Delayms(5);
  LCD_Write_Command(0x0c);Delayms(5);
}
//设置显示位置
void LCD_Pos(unsigned char pos)
{
  LCD_Write_Command(pos|0x80);
}
/*
//T0的中断服务子函数
void T0Deal()interrupt 1
{
 TH0=-600/256;
 TL0=-600%256;
 if(++tCount==200)
 {
  tCount=0;
  TR0=0;
 }
}
*/
//按键扫描函数，返回值为对应的按键编码
unsigned char GetKey()
{
 unsigned char i,j,k=0;
 unsigned char KeyScanCode[]={0xef,0xdf,0xbf,0x7f};   //行列扫描的行列码
 unsigned char KeyCodeTable[]={
 0xee,0xed,0xeb,0xde,0xdd,0xdb,0xbe,0xbd,0xbb,0x7e,0x7d,0x7b};
 P3=0x0f;
 if(P3!=0x0f)       //如果有按键被按下
 {
  for(i=0;i<4;i++)    //依次进行扫描
  {
    P3=KeyScanCode[i];
    for(j=0;j<3;j++)
    {
      k=i*3+j;                //计算对应的按键编码
      if(P3==KeyCodeTable[k])
      {
        return k;             //返回按键编码
      }
     }
  }
 }
 else 
 {
  return 0xff;                  //或者返回0xff
 }
}
//主函数
void main()
{
 unsigned char i=0,j;
 P0 = 0xFF;
 P2 = 0xFF;
 P1 = 0xFF;                   //初始化端口
// IE=0x82;                     //设置中断
// TMOD=0x01;                   //启动T0定时器
 Init_LCD();                  //初始化1602
 Display_String(Title_Text,0x00); //显示 --Phone Code--
 while(1)
 {
  KeyNo = GetKey();           //获得按键状态
  if(KeyNo==0xff)
  {
    continue;                  //如果没有按键，则进入下一个循环
  }
  if(++i==12)                 //如果已经超过11个数字，清除显示屏幕
  {
    for(j=0;j<16;j++)
    Dial_Code_Str[j]=' ';
    i=0;
  }
  Dial_Code_Str[i]=Key_Table[KeyNo];  //显示拨号数据
  Display_String(Dial_Code_Str,0x40);
//  TR0=1;
  while(GetKey()!=0xff);
 }
}
