#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 
#include "stmflash.h"
#include "rs485.h"

//----------------------------------------------------------------//
//                      智能门禁系统                              //
//                      华南理工大学                              //
//                      版本：4.0                                 //
//                      作者：陈艺荣、周泽鑫、吴子莹              //
//                            何晨晖、陈靖康                      //
//                      邮箱：eecyryou@mail.scut.edu.cn           //
//----------------------------------------------------------------//

//--------------------------------------------------------------------------------------------
// 蓝牙通信指令                                实现功能
// -------------------------------------------------------------------------------------------
// root用户Android ID                        登记root用户，只在首次使用或者格式化后使用有效，否则无效
// 'R'+root用户Android ID+'0'                PF10置0，LED1亮，关门
// 'R'+root用户Android ID+'1'                PF10置1，LED1暗，开门
// 'C'+普通用户Android ID+'0'                PF10置0，LED1亮，关门
// 'C'+普通用户Android ID+'1'                PF10置1，LED1暗，开门
// root用户Android ID+'T'+普通用户Android ID 添加普通用户
// root用户Android ID+'S'+普通用户Android ID 删除普通用户
//
//--------------------------------------------------------------------------------------------
// 按键                                         实现功能
// 红色复位键                                复位           
// KEY0                                      强行初始化Android ID
// KEY1                                      格式化所有用户记录
// KEY_UP     
// KEY2  
//--------------------------------------------------------------------------------------------

// 修改时间：2018年07月05日

// 设置FLASH 保存地址(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.
// 否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.
#define FLASH_DATA_SAVE    0X08040000

// 0~3:num
// 4~19:root user Android ID
// 20~35:common user Android ID 1
// ...
// 132~147:common user Android ID 8
u8 flash_data_save[148];   

// 用于存放root ID
const u8 id_root_buf[]={"7659483c763e8649"};
// 用户id索引，其中find_id[0]为root用户id
u8 find_id[9]={4,20,36,52,68,84,100,116,132};

#define COMMON_BUF_LENTH sizeof(id_common_buf)
#define ROOT_BUF_LENTH sizeof(id_root_buf)
#define COMMON_BUF_SIZE COMMON_BUF_LENTH/4+((COMMON_BUF_LENTH%4)?1:0)
#define ROOT_BUF_SIZE ROOT_BUF_LENTH/4+((ROOT_BUF_LENTH%4)?1:0)


//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
  {
	if(HC05_Get_Role()==1)LCD_ShowString(30,160,200,16,16,"ROLE:Master");	//主机
	else LCD_ShowString(30,160,200,16,16,"ROLE:Slave ");			 		//从机
}
//显示ATK-HC05模块的连接状态
void HC05_Sta_Show(void)
{												 
	POINT_COLOR=RED;
	if(HC05_LED)LCD_ShowString(120,160,120,16,16,"STA:Connected ");			//连接成功
	else LCD_ShowString(120,160,120,16,16,"STA:Disconnect");	 			//未连接				 
}	 


int main(void)
{ 
  // FLASH_EraseAllSectors(VoltageRange_3);
	// HC05_Set_Cmd("AT+NAME=SCUT-Beisan822");
	
	u8 i,j,temp;
	u8 time=0;
	u8 flag=1;
	u8 kaimenflag=1;
	u8 t=0;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];
	u8 len=0;
	u8 rs485buf[5]; 
	u8 reclen=0;
  u8 num;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);        //初始化延时函数
	uart_init(115200);
	RS485_Init(9600);		//初始化RS485串口2
	//uart_init(115200);		  //初始化串口波特率为115200
	usmart_dev.init(84); 		//初始化USMART		
	LED_Init();					    //初始化LED
	KEY_Init();					    //初始化按键
 	LCD_Init();		 			    //初始化LCD	  
	POINT_COLOR=RED;

	LCD_ShowString(30,30,200,16,16,"Dianzixitongzonghesheji");	
	LCD_ShowString(30,50,200,16,16,"Zhineng menjin xitong");
  POINT_COLOR=BLUE;	
	LCD_ShowString(30,70,200,16,16,"eecyryou@mail.scut.edu.cn");
	POINT_COLOR=RED;
	delay_ms(1000);			   //等待蓝牙模块上电稳定
	LED1=0;                //上电时默认关门，PF10
 	while(HC05_Init()) 		 //初始化ATK-HC05模块  
	{
		LCD_ShowString(30,110,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,110,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	 										   	   
	LCD_ShowString(30,110,500,16,16,"KEY_UP:ROLE     KEY0:ROOT     KEY1:FORMATTING");  
	LCD_ShowString(30,130,200,16,16,"ATK-HC05 Standby!");  
	LCD_ShowString(30,550,200,16,16,"Receive:");
	LCD_ShowString(30,600,200,16,16,"User Number:");
  LCD_ShowString(30,180,200,16,16,"Class:");
	LCD_ShowString(30,200,200,16,16,"Author:");
	POINT_COLOR=BLUE;
	LCD_ShowString(90,180,200,16,16,"15Diankezhuoyueban");
	LCD_ShowString(90,200,200,16,16,"Chen Yirong");
	LCD_ShowString(90,220,200,16,16,"Zhou Zexin");
	LCD_ShowString(90,240,200,16,16,"He Chenhui");
	LCD_ShowString(90,260,200,16,16,"Wu Ziying");
	LCD_ShowString(90,280,200,16,16,"Chen Jingkang");

  HC05_Set_Cmd("AT+NAME=SCUT-Beisan822");
	
	POINT_COLOR=BLUE;
	HC05_Role_Show();
	delay_ms(100);
	USART3_RX_STA=0;
	
	
 	while(1) 
	{		
        
		HC05_Sta_Show();
		key=KEY_Scan(0);  //调整蓝牙模式等
		if(key==WKUP_PRES)						//切换模块主从设置
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//状态取反	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//复位ATK-HC05模块
				delay_ms(200);
				
			}
		}else if(key==KEY0_PRES)  //初始化
		{
			flash_data_save[0]=1;  //记录保存的Android_ID个数，初始化为1
			num=flash_data_save[0];
      LCD_ShowxNum(130,600,num,4,16,0);
			for(j=0;j<9;j++)   //初始化时，全部id设置为root用户的id
			{
				for(i=0;i<16;i++)
					{ //初始化ROOT
						temp=find_id[j]+i;					
						flash_data_save[temp]=id_root_buf[i];
											
					}
      }				
			LCD_ShowString(30,500,500,16,16,"Message:Start Write FLASH....");
			STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
			LCD_ShowString(30,500,500,16,16,"Message:Initial Finished!");//提示传送完成
		
		}else if(key==KEY1_PRES)   //格式化
		{
			flash_data_save[0]=0;  //记录保存的Android_ID个数，格式化为0			
			num=flash_data_save[0];
      LCD_ShowxNum(130,600,num,4,16,0);
			LCD_ShowString(30,500,500,16,16,"Message:Start Write FLASH....");
			STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
			LCD_ShowString(30,500,500,16,16,"Message:Initial Finished!");//提示传送完成
			LCD_ShowString(30,500,500,16,16,"Message:Format Finished!"); //格式化完毕
		
		}else if(key==KEY2_PRES)   //待开发
		{
			LCD_ShowString(30,500,500,16,16,"Message:Way for developing!"); //格式化完毕
		
		}else delay_ms(10);	 
		
		if(t==10)  //闪烁红色LED
		{
			if(sendmask)					//定时发送
			{
				sprintf((char*)sendbuf,"ALIENTEK HC05 %d\r\n",sendcnt);
	  			LCD_ShowString(30+40,160,200,16,16,sendbuf);	//显示发送数据	
				u3_printf("ALIENTEK HC05 %d\r\n",sendcnt);		//发送到蓝牙模块
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}
		if(time==300)    //无操作一段时间后执行关门
			{
				LED1=0;
				time=0;
				
				if(LED1==0)
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//清除显示
						 POINT_COLOR=RED;
						 LCD_ShowString(140,650,200,16,16,"Door Close!");
						 
					 }
					 else
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//清除显示
						 POINT_COLOR=BLUE;
						 LCD_ShowString(140,650,200,16,16,"Door Open!");
					 }
			}

		if(USART3_RX_STA&0X8000)			//蓝牙接收到一次数据了，手机与STM32通信
		{
			LCD_Fill(100,550,400,240,WHITE);	//清除显示
 			reclen=USART3_RX_STA&0X7FFF;	//得到数据长度
			USART3_RX_BUF[reclen]=0;	 	//加入结束符
			LCD_Fill(100,550,500,580,WHITE);	//清除显示
			LCD_ShowString(100,550,300,16,16,USART3_RX_BUF);//显示接收到的数据
 			USART3_RX_STA=0;

			if(reclen==16)     //登记root用户
			{
				STMFLASH_Read(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
				num=flash_data_save[0];
        LCD_ShowxNum(130,600,num,4,16,0);
        if(num==0)
				{
						flash_data_save[0]=1;  //记录保存的Android_ID个数，初始化为1
						num=flash_data_save[0];
            LCD_ShowxNum(130,600,num,4,16,0);
					  //flash_data_save[148]
						for(j=0;j<9;j++)   //初始化时，全部id设置为root用户的id
						{
							for(i=0;i<16;i++)
								{ //初始化ROOT
									temp=find_id[j]+i;					
									flash_data_save[temp]=USART3_RX_BUF[i];											
								}
						}				
						LCD_ShowString(30,500,500,16,16,"Message:Start Write FLASH....");
						STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
						LCD_ShowString(30,500,500,16,16,"Message:Initial root user Finished!");//提示传送完成		
				}
        else LCD_ShowString(30,500,500,16,16,"Error:Root user has existed,could not change!");//提示传送完成						
			}

			if(reclen==18||reclen==17)   //指令长度为18，格式：R/C+16位Android_ID+0/1,R: C: 0: 1 
			 {
				STMFLASH_Read(FLASH_DATA_SAVE,(u32*)flash_data_save,37);				 
				 if(USART3_RX_BUF[0]==82)   //R:82
				{
					for(i=0;i<16;i++)
						 {
							 
							 if(flash_data_save[i+find_id[0]]!=USART3_RX_BUF[i+1]) kaimenflag=0;
						 }
				  if(kaimenflag==1)
					{
						LCD_Fill(30,500,400,550,WHITE);	//清除显示
						LCD_ShowString(30,500,500,16,16,"Message:root use successful!");
						if(USART3_RX_BUF[17]==48) LED1=0; //关门
							
						if(USART3_RX_BUF[17]==49) LED1=1,time=0; //开门

					}
					kaimenflag=1;
				}
				if(USART3_RX_BUF[0]==67)   //C:67
				{
					for(j=num;j>0;j--)
					{
						for(i=0;i<16;i++)
							 {
								 if(flash_data_save[i+find_id[j]]!=USART3_RX_BUF[i+1]) break;
								 else if(i==15)
											 {
												 LCD_Fill(30,500,400,550,WHITE);	//清除显示
												 LCD_ShowString(30,500,500,16,16,"Message:common use successful!");
													if(USART3_RX_BUF[17]==48) LED1=0; //关门,0:48
														
													if(USART3_RX_BUF[17]==49) LED1=1,time=0; //开门,1:49
											 }							 
							 }
					 }
				}		
						if(LED1==0)
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//清除显示
						 POINT_COLOR=RED;
						 LCD_ShowString(140,650,200,16,16,"Door Close!");
						 
					 }
					 else
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//清除显示
						 POINT_COLOR=BLUE;
						 LCD_ShowString(140,650,200,16,16,"Door Open!");
					 }

			}
				
			if(reclen==33)   //添加或删除用户
			{
				STMFLASH_Read(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
				num=flash_data_save[0];
        LCD_ShowxNum(130,600,num,4,16,0);
        //判断是否为root用户操作
				for(i=0;i<16;i++)
				  {
					if(!((USART3_RX_BUF[i]==flash_data_save[i+find_id[0]])))
					  {
							LCD_ShowString(30,500,500,16,16,"Error:Sorry,you are mot my manager!");
					   flag=0;
					  }
				  }
				//是root用户	
				  if(flag)
						 { //root用户存入一个新的common id
							 LCD_ShowString(30,500,500,25,25,"Message:HELLO,my manager!");
							 if(USART3_RX_BUF[16]=='T')   //添加用户
							 {										
									if(num<8)
									{
										 for(i=0;i<16;i++)
												 {
													 flash_data_save[i+find_id[num]]=USART3_RX_BUF[17+i];
												 }
											num=num+1;   //普通用户数目增加1 
											flash_data_save[0]=num;
											STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);	 
											LCD_ShowString(30,500,500,16,16,"Message:Add User Successfully!");//提示传送完成	 										
									}
									else LCD_ShowString(30,500,500,25,25,"Error:User registered full!");							 
						   
							 }
							 if(USART3_RX_BUF[16]=='S')   //删除用户
							 {
									u8 delete_temp=0;								 
									if(num>1)
									{
										 for(j=num;j>=1;j--)
										{
											 for(i=0;i<16;i++)
													 {
														 if(flash_data_save[i+find_id[j]]!=USART3_RX_BUF[17+i]) break;
														 if(i==15) delete_temp=j;
													 }
											if(delete_temp>0) break;		 
										};
											if(delete_temp>0)
											{
												for(i=0;i<16;i++)   //将第num-1个common用户的id移到被删除用户的id那里
														 {
															 flash_data_save[i+find_id[delete_temp]]=flash_data_save[i+find_id[num]];
														 }
												for(i=0;i<16;i++)   
														 {
															 flash_data_save[i+find_id[num]]=flash_data_save[i+find_id[0]]; //用root用户id覆盖最后的位置
														 }		 
												num=num-1;   //普通用户数目减少1
												flash_data_save[0]=num;	
												STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);	 
												LCD_ShowString(30,500,200,16,16,"Delete User Finished!");//提示传送完成	 												 
												
											}
											else
											{
												LCD_ShowString(30,500,200,16,16,"Error:No this user!");//没有要删除的用户
											}
									}
									else LCD_ShowString(30,500,210,25,25,"Error:No common user!"); //没有普通用户										 
							 }
		 
						 }
			}

		}
		 t++;   //计时
		 time++;	//计时关门
//---------------------------------------------------------------------------------------
//  语音模块
//  陈艺荣		
		RS485_Receive_Data(rs485buf,&len);
		if(len)//接收到有数据，语音模块与STM32通信
		{
			if(len>5)len=5;//最大是5个数据.
 			//for(i=0;i<len;i++) LCD_ShowxNum(30+i*32,500,rs485buf[i],3,16,0X80);	//显示数据
 		  
			if(rs485buf[0]==16&&rs485buf[1]==13)  LED1=1,time=0; //开门,1:49
			if(rs485buf[0]==1&&rs485buf[1]==13) LED1=0;  //关门
		  
			if(LED1==0)
				 {
					 LCD_Fill(140,650,300,700,WHITE);	//清除显示
					 POINT_COLOR=RED;
					 LCD_ShowString(140,650,200,16,16,"Door Close!");
					 
				 }
				 else
				 {
					 LCD_Fill(140,650,300,700,WHITE);	//清除显示
					 POINT_COLOR=BLUE;
					 LCD_ShowString(140,650,200,16,16,"Door Open!");
				 }
		
		
		}
		

		
		
		
	}											    
}


