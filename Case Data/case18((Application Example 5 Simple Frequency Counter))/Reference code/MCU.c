#include <AT89X52.H> 
unsigned char code dispbit[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};     //P2的扫描位
unsigned char code dispcode[]={0x3f,0x06,0x5b,0x4f,0x66, 
                               0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x40};    //数码管的字形编码
unsigned char dispbuf[8]={0,0,0,0,0,0,10,10}; //初始化显示值
unsigned char temp[8];     //存放显示的数据
unsigned char dispcount;   //显示计数器值
unsigned char T0count;     //T0的计数器值
unsigned char timecount;   //计时计数器值
bit flag;                  //标志位
unsigned long x;           //频率值
//频率计算函数
void HzCal(void)
{
  unsigned char i; 
  x=T0count*65536+TH0*256+TL0; //得到T0的16位计数器值
  for(i=0;i<8;i++) 
  { 
    temp[i]=0; 
  } 
         i=0; 
         while(x/10)         //拆分
           { 
             temp[i]=x%10; 
             x=x/10; 
             i++; 
           } 
         temp[i]=x; 
         for(i=0;i<6;i++)    //换算为显示数据
           { 
             dispbuf[i]=temp[i]; 
           } 
         timecount=0; 
         T0count=0; 
}
  
void main(void) 
{
  
  TMOD=0x15;              //设置定时器工作方式
  TH0=0; 
  TL0=0; 
  TH1=(65536-5000)/256; 
  TL1=(65536-5000)%256;       //初始化T1
  TR1=1; 
  TR0=1; 
  ET0=1; 
  ET1=1; 
  EA=1;                     //开中断
  
  while(1) 
   { 
     if(flag==1) 
       { 
         flag=0; 
         HzCal();   //频率计算函数
         TH0=0; 
         TL0=0; 
         TR0=1; 
       } 
   } 
} 
//定时器T0中断服务子函数
void t0(void) interrupt 1 using 0 
{ 
  T0count++; 
} 
//定时器T1中断服务子函数  
void t1(void) interrupt 3 using 0 
{ 
  TH1=(65536-5000)/256; 
  TL1=(65536-5000)%256;          //初始化T1预装值，1ms定时
  timecount++;                   //扫描
  if(timecount==200)             //秒定时
    { 
      TR0=0;                     //启动T0
      timecount=0; 
      flag=1; 
    } 
  P2=0xff;                         //初始化选择引脚
  P0=dispcode[dispbuf[dispcount]]; //输出待显示数据
  P2=dispbit[dispcount]; 
  dispcount++;                    //切换到下一个选择引脚
  if(dispcount==8)                //如果已经扫描完成切换
    { 
      dispcount=0; 
    } 
} 

