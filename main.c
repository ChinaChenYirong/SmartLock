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
//                      �����Ž�ϵͳ                              //
//                      ��������ѧ                              //
//                      �汾��4.0                                 //
//                      ���ߣ������١������Ρ�����Ө              //
//                            �γ��͡��¾���                      //
//                      ���䣺eecyryou@mail.scut.edu.cn           //
//----------------------------------------------------------------//

//--------------------------------------------------------------------------------------------
// ����ͨ��ָ��                                ʵ�ֹ���
// -------------------------------------------------------------------------------------------
// root�û�Android ID                        �Ǽ�root�û���ֻ���״�ʹ�û��߸�ʽ����ʹ����Ч��������Ч
// 'R'+root�û�Android ID+'0'                PF10��0��LED1��������
// 'R'+root�û�Android ID+'1'                PF10��1��LED1��������
// 'C'+��ͨ�û�Android ID+'0'                PF10��0��LED1��������
// 'C'+��ͨ�û�Android ID+'1'                PF10��1��LED1��������
// root�û�Android ID+'T'+��ͨ�û�Android ID �����ͨ�û�
// root�û�Android ID+'S'+��ͨ�û�Android ID ɾ����ͨ�û�
//
//--------------------------------------------------------------------------------------------
// ����                                         ʵ�ֹ���
// ��ɫ��λ��                                ��λ           
// KEY0                                      ǿ�г�ʼ��Android ID
// KEY1                                      ��ʽ�������û���¼
// KEY_UP     
// KEY2  
//--------------------------------------------------------------------------------------------

// �޸�ʱ�䣺2018��07��05��

// ����FLASH �����ַ(����Ϊż��������������,Ҫ���ڱ�������ռ�õ�������.
// ����,д������ʱ��,���ܻᵼ�²�����������,�Ӷ����𲿷ֳ���ʧ.��������.
#define FLASH_DATA_SAVE    0X08040000

// 0~3:num
// 4~19:root user Android ID
// 20~35:common user Android ID 1
// ...
// 132~147:common user Android ID 8
u8 flash_data_save[148];   

// ���ڴ��root ID
const u8 id_root_buf[]={"7659483c763e8649"};
// �û�id����������find_id[0]Ϊroot�û�id
u8 find_id[9]={4,20,36,52,68,84,100,116,132};

#define COMMON_BUF_LENTH sizeof(id_common_buf)
#define ROOT_BUF_LENTH sizeof(id_root_buf)
#define COMMON_BUF_SIZE COMMON_BUF_LENTH/4+((COMMON_BUF_LENTH%4)?1:0)
#define ROOT_BUF_SIZE ROOT_BUF_LENTH/4+((ROOT_BUF_LENTH%4)?1:0)


//��ʾATK-HC05ģ�������״̬
void HC05_Role_Show(void)
  {
	if(HC05_Get_Role()==1)LCD_ShowString(30,160,200,16,16,"ROLE:Master");	//����
	else LCD_ShowString(30,160,200,16,16,"ROLE:Slave ");			 		//�ӻ�
}
//��ʾATK-HC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	POINT_COLOR=RED;
	if(HC05_LED)LCD_ShowString(120,160,120,16,16,"STA:Connected ");			//���ӳɹ�
	else LCD_ShowString(120,160,120,16,16,"STA:Disconnect");	 			//δ����				 
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);        //��ʼ����ʱ����
	uart_init(115200);
	RS485_Init(9600);		//��ʼ��RS485����2
	//uart_init(115200);		  //��ʼ�����ڲ�����Ϊ115200
	usmart_dev.init(84); 		//��ʼ��USMART		
	LED_Init();					    //��ʼ��LED
	KEY_Init();					    //��ʼ������
 	LCD_Init();		 			    //��ʼ��LCD	  
	POINT_COLOR=RED;

	LCD_ShowString(30,30,200,16,16,"Dianzixitongzonghesheji");	
	LCD_ShowString(30,50,200,16,16,"Zhineng menjin xitong");
  POINT_COLOR=BLUE;	
	LCD_ShowString(30,70,200,16,16,"eecyryou@mail.scut.edu.cn");
	POINT_COLOR=RED;
	delay_ms(1000);			   //�ȴ�����ģ���ϵ��ȶ�
	LED1=0;                //�ϵ�ʱĬ�Ϲ��ţ�PF10
 	while(HC05_Init()) 		 //��ʼ��ATK-HC05ģ��  
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
		key=KEY_Scan(0);  //��������ģʽ��
		if(key==WKUP_PRES)						//�л�ģ����������
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//״̬ȡ��	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//��λATK-HC05ģ��
				delay_ms(200);
				
			}
		}else if(key==KEY0_PRES)  //��ʼ��
		{
			flash_data_save[0]=1;  //��¼�����Android_ID��������ʼ��Ϊ1
			num=flash_data_save[0];
      LCD_ShowxNum(130,600,num,4,16,0);
			for(j=0;j<9;j++)   //��ʼ��ʱ��ȫ��id����Ϊroot�û���id
			{
				for(i=0;i<16;i++)
					{ //��ʼ��ROOT
						temp=find_id[j]+i;					
						flash_data_save[temp]=id_root_buf[i];
											
					}
      }				
			LCD_ShowString(30,500,500,16,16,"Message:Start Write FLASH....");
			STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
			LCD_ShowString(30,500,500,16,16,"Message:Initial Finished!");//��ʾ�������
		
		}else if(key==KEY1_PRES)   //��ʽ��
		{
			flash_data_save[0]=0;  //��¼�����Android_ID��������ʽ��Ϊ0			
			num=flash_data_save[0];
      LCD_ShowxNum(130,600,num,4,16,0);
			LCD_ShowString(30,500,500,16,16,"Message:Start Write FLASH....");
			STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
			LCD_ShowString(30,500,500,16,16,"Message:Initial Finished!");//��ʾ�������
			LCD_ShowString(30,500,500,16,16,"Message:Format Finished!"); //��ʽ�����
		
		}else if(key==KEY2_PRES)   //������
		{
			LCD_ShowString(30,500,500,16,16,"Message:Way for developing!"); //��ʽ�����
		
		}else delay_ms(10);	 
		
		if(t==10)  //��˸��ɫLED
		{
			if(sendmask)					//��ʱ����
			{
				sprintf((char*)sendbuf,"ALIENTEK HC05 %d\r\n",sendcnt);
	  			LCD_ShowString(30+40,160,200,16,16,sendbuf);	//��ʾ��������	
				u3_printf("ALIENTEK HC05 %d\r\n",sendcnt);		//���͵�����ģ��
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}
		if(time==300)    //�޲���һ��ʱ���ִ�й���
			{
				LED1=0;
				time=0;
				
				if(LED1==0)
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//�����ʾ
						 POINT_COLOR=RED;
						 LCD_ShowString(140,650,200,16,16,"Door Close!");
						 
					 }
					 else
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//�����ʾ
						 POINT_COLOR=BLUE;
						 LCD_ShowString(140,650,200,16,16,"Door Open!");
					 }
			}

		if(USART3_RX_STA&0X8000)			//�������յ�һ�������ˣ��ֻ���STM32ͨ��
		{
			LCD_Fill(100,550,400,240,WHITE);	//�����ʾ
 			reclen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
			USART3_RX_BUF[reclen]=0;	 	//���������
			LCD_Fill(100,550,500,580,WHITE);	//�����ʾ
			LCD_ShowString(100,550,300,16,16,USART3_RX_BUF);//��ʾ���յ�������
 			USART3_RX_STA=0;

			if(reclen==16)     //�Ǽ�root�û�
			{
				STMFLASH_Read(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
				num=flash_data_save[0];
        LCD_ShowxNum(130,600,num,4,16,0);
        if(num==0)
				{
						flash_data_save[0]=1;  //��¼�����Android_ID��������ʼ��Ϊ1
						num=flash_data_save[0];
            LCD_ShowxNum(130,600,num,4,16,0);
					  //flash_data_save[148]
						for(j=0;j<9;j++)   //��ʼ��ʱ��ȫ��id����Ϊroot�û���id
						{
							for(i=0;i<16;i++)
								{ //��ʼ��ROOT
									temp=find_id[j]+i;					
									flash_data_save[temp]=USART3_RX_BUF[i];											
								}
						}				
						LCD_ShowString(30,500,500,16,16,"Message:Start Write FLASH....");
						STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
						LCD_ShowString(30,500,500,16,16,"Message:Initial root user Finished!");//��ʾ�������		
				}
        else LCD_ShowString(30,500,500,16,16,"Error:Root user has existed,could not change!");//��ʾ�������						
			}

			if(reclen==18||reclen==17)   //ָ���Ϊ18����ʽ��R/C+16λAndroid_ID+0/1,R: C: 0: 1 
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
						LCD_Fill(30,500,400,550,WHITE);	//�����ʾ
						LCD_ShowString(30,500,500,16,16,"Message:root use successful!");
						if(USART3_RX_BUF[17]==48) LED1=0; //����
							
						if(USART3_RX_BUF[17]==49) LED1=1,time=0; //����

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
												 LCD_Fill(30,500,400,550,WHITE);	//�����ʾ
												 LCD_ShowString(30,500,500,16,16,"Message:common use successful!");
													if(USART3_RX_BUF[17]==48) LED1=0; //����,0:48
														
													if(USART3_RX_BUF[17]==49) LED1=1,time=0; //����,1:49
											 }							 
							 }
					 }
				}		
						if(LED1==0)
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//�����ʾ
						 POINT_COLOR=RED;
						 LCD_ShowString(140,650,200,16,16,"Door Close!");
						 
					 }
					 else
					 {
						 LCD_Fill(140,650,300,700,WHITE);	//�����ʾ
						 POINT_COLOR=BLUE;
						 LCD_ShowString(140,650,200,16,16,"Door Open!");
					 }

			}
				
			if(reclen==33)   //��ӻ�ɾ���û�
			{
				STMFLASH_Read(FLASH_DATA_SAVE,(u32*)flash_data_save,37);
				num=flash_data_save[0];
        LCD_ShowxNum(130,600,num,4,16,0);
        //�ж��Ƿ�Ϊroot�û�����
				for(i=0;i<16;i++)
				  {
					if(!((USART3_RX_BUF[i]==flash_data_save[i+find_id[0]])))
					  {
							LCD_ShowString(30,500,500,16,16,"Error:Sorry,you are mot my manager!");
					   flag=0;
					  }
				  }
				//��root�û�	
				  if(flag)
						 { //root�û�����һ���µ�common id
							 LCD_ShowString(30,500,500,25,25,"Message:HELLO,my manager!");
							 if(USART3_RX_BUF[16]=='T')   //����û�
							 {										
									if(num<8)
									{
										 for(i=0;i<16;i++)
												 {
													 flash_data_save[i+find_id[num]]=USART3_RX_BUF[17+i];
												 }
											num=num+1;   //��ͨ�û���Ŀ����1 
											flash_data_save[0]=num;
											STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);	 
											LCD_ShowString(30,500,500,16,16,"Message:Add User Successfully!");//��ʾ�������	 										
									}
									else LCD_ShowString(30,500,500,25,25,"Error:User registered full!");							 
						   
							 }
							 if(USART3_RX_BUF[16]=='S')   //ɾ���û�
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
												for(i=0;i<16;i++)   //����num-1��common�û���id�Ƶ���ɾ���û���id����
														 {
															 flash_data_save[i+find_id[delete_temp]]=flash_data_save[i+find_id[num]];
														 }
												for(i=0;i<16;i++)   
														 {
															 flash_data_save[i+find_id[num]]=flash_data_save[i+find_id[0]]; //��root�û�id��������λ��
														 }		 
												num=num-1;   //��ͨ�û���Ŀ����1
												flash_data_save[0]=num;	
												STMFLASH_Write(FLASH_DATA_SAVE,(u32*)flash_data_save,37);	 
												LCD_ShowString(30,500,200,16,16,"Delete User Finished!");//��ʾ�������	 												 
												
											}
											else
											{
												LCD_ShowString(30,500,200,16,16,"Error:No this user!");//û��Ҫɾ�����û�
											}
									}
									else LCD_ShowString(30,500,210,25,25,"Error:No common user!"); //û����ͨ�û�										 
							 }
		 
						 }
			}

		}
		 t++;   //��ʱ
		 time++;	//��ʱ����
//---------------------------------------------------------------------------------------
//  ����ģ��
//  ������		
		RS485_Receive_Data(rs485buf,&len);
		if(len)//���յ������ݣ�����ģ����STM32ͨ��
		{
			if(len>5)len=5;//�����5������.
 			//for(i=0;i<len;i++) LCD_ShowxNum(30+i*32,500,rs485buf[i],3,16,0X80);	//��ʾ����
 		  
			if(rs485buf[0]==16&&rs485buf[1]==13)  LED1=1,time=0; //����,1:49
			if(rs485buf[0]==1&&rs485buf[1]==13) LED1=0;  //����
		  
			if(LED1==0)
				 {
					 LCD_Fill(140,650,300,700,WHITE);	//�����ʾ
					 POINT_COLOR=RED;
					 LCD_ShowString(140,650,200,16,16,"Door Close!");
					 
				 }
				 else
				 {
					 LCD_Fill(140,650,300,700,WHITE);	//�����ʾ
					 POINT_COLOR=BLUE;
					 LCD_ShowString(140,650,200,16,16,"Door Open!");
				 }
		
		
		}
		

		
		
		
	}											    
}


