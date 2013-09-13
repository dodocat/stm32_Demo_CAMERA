/*
GUANFU_WANG@126.COM
QQ:21578936
未经许可禁止任何商业用途
*/
/*
OV7725
*/
#include "ov7670.h"
#include "main.h"
#include "ov7670config.h"
#include "stm32f10x_lib.h"


////////////////////////////
//功能：提供7660时钟
//返回：无
/*
void XCLK_init_ON(void)
{
  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    RCC_MCOConfig(RCC_MCO_HSE  );//hsi
}
void XCLK_init_OFF(void)
{
  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
*/
void ov7670_GPIO_Init(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
   // Enable GPIOC  clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  OV7670_VSYNC_BIT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 
}

/********************************************************************
函数功能：7670中断配制
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void ov7670_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI9_5 Interrupt 
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  */
  
  
    /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  

  

}



void OV7670_EXTI_Config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  
 // GPIO_EXTILineConfig(GPIO_PORT_SOURCE_HREF_CMOS, GPIO_PIN_SOURCE_HREF_CMOS);
 /// EXTI_InitStructure.EXTI_Line = EXTI_LINE_HREF_CMOS;
 /// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
 // EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
 // EXTI_InitStructure.EXTI_LineCmd = ENABLE;
 // EXTI_Init(&EXTI_InitStructure);
//  EXTI_GenerateSWInterrupt(EXTI_LINE_HREF_CMOS);
  

  GPIO_EXTILineConfig(GPIO_PORT_SOURCE_VSYNC_CMOS, GPIO_PIN_SOURCE_VSYNC_CMOS);
  EXTI_InitStructure.EXTI_Line = EXTI_LINE_VSYNC_CMOS;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  
  
  
  #if   CMOS_MD==0 
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ;
#elif CMOS_MD==1
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ;
#elif CMOS_MD==2
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ;
#elif CMOS_MD==3
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ;
#endif
 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  EXTI_GenerateSWInterrupt(EXTI_LINE_VSYNC_CMOS);

}



/*
//(140,16,640,480) is good for VGA
//(272,16,320,240) is good for QVGA

void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height)
{
	unsigned int endx;
	unsigned int endy;// "v*2"必须
	unsigned char temp_reg1, temp_reg2;
	unsigned char state,temp;
	
	endx=(startx+width);
	endy=(starty+height);// "v*2"必须
	state = rdOV7670Reg(0x03, &temp_reg1 );
	temp_reg1 &= 0xf0;
	state = rdOV7670Reg(0x32, &temp_reg2 );
	temp_reg2 &= 0xc0;
	
	// Horizontal
	temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
	state = wrOV7670Reg(0x32, temp );
	temp = (startx&0x7F8)>>3;
	state = wrOV7670Reg(0x17, temp );
	temp = (endx&0x7F8)>>3;
	state = wrOV7670Reg(0x18, temp );
	
	// Vertical
	temp = temp_reg1|((endy&0x3)<<2)|(starty&0x3);
	state = wrOV7670Reg(0x03, temp );
	temp = starty>>2;
	state = wrOV7670Reg(0x19, temp );
	temp = endy>>2;
	state = wrOV7670Reg(0x1A, temp );
}

*/


////////////////////////////
//功能：写OV7660寄存器
//返回：1-成功	0-失败
#if  (CMOS_MD==0 )|(CMOS_MD==1 )
    unsigned char wrOV7670Reg(unsigned char regID, unsigned char regDat)
    {
		startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regDat))
	{
		stopSCCB();
		return(0);
	}
  	stopSCCB();
	
  	return(1);
     }
#elif (CMOS_MD==2 )|(CMOS_MD==3 )
   unsigned char wrOV7670Reg(unsigned char Page,unsigned char regID, unsigned int regDat)
   {     
      
        startSCCB();
	if(0==SCCBwriteByte(0xba))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(0xf0))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte((unsigned char)( Page>>8)))
	{
		stopSCCB();
		return(0);
	}
        if(0==SCCBwriteByte((unsigned char)( Page)))
	{
		stopSCCB();
		return(0);
	}
        stopSCCB();
   //////////////////////////////////////////////////////////////     
	startSCCB();
	if(0==SCCBwriteByte(0xba))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte((unsigned char)(regDat>>8)))
	{
		stopSCCB();
		return(0);
	}
        
   	if(0==SCCBwriteByte((unsigned char)regDat))
	{
		stopSCCB();
		return(0);
	}
        
  	stopSCCB();
	
  	return(1);
}
  #endif 
////////////////////////////
//功能：读OV7660寄存器
//返回：1-成功	0-失败
unsigned char rdOV7670Reg(unsigned char regID, unsigned char *regDat)
{
	//通过写操作设置寄存器地址
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	stopSCCB();
	
	delay_us(100);
	
	//设置寄存器地址后，才是读
	startSCCB();
	if(0==SCCBwriteByte(0x43))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	*regDat=SCCBreadByte();
  	noAck();
  	stopSCCB();
  	return(1);
}




/* OV7670_init() */
//返回1成功，返回0失败
unsigned char OV7670_init(void)
{
	unsigned char temp;
	
	unsigned int i=0;

	//uchar ovidmsb=0,ovidlsb=0;
	ov7670_GPIO_Init();
	SCCB_GPIO_Config();//io init..
 #if   (CMOS_MD==0 )| (CMOS_MD==1 )


	temp=0x80;
	if(0==wrOV7670Reg(0x12, temp)) //Reset SCCB
	{
          return 0 ;
	}
	Delay(50);

        
        for(i=0;i<CHANGE_REG_NUM;i++)
	{
		if( 0==wrOV7670Reg(change_reg[i][0],change_reg[i][1]))
		{
			return 0;
		}
	}
       // OV7670_config_window(272,16,128,128);
  #elif(CMOS_MD==2 )| (CMOS_MD==3)   
        for(i=0;i<CHANGE_REG_NUM;i++)
	{
	       if(change_reg[i][0]>2)
               {
                  Delay(change_reg[i][2]); // Delay 5ms();
               }
               else
               {
                  if( 0==wrOV7670Reg(change_reg[i][0],change_reg[i][1],change_reg[i][2]))
		  {
			return 0;
		  }
               }
             // delay_us(10);
	}
        
  #endif      
	return 0x01; //ok
} 

