#include "Nano100Series.h"              // Device header
#include <stdio.h>

void lcd_print(uint8_t pos, char *s);
void lcd_init(void);
 
//------------------- LCD ------------------
#define RS  PC15
#define RW  PC14
#define E   PB15
 
static void delay(uint32_t t){
    int i;
    for(i=0; i < t; i++){}
}
//---
void lcd_cmd(uint8_t c){
    RS=0; RW=0;
    PD->DOUT = c;
    delay( 10 ); E=1; delay( 10 ); E=0;
    delay( 100 );
}
 
//---
void lcd_put(uint8_t c){
    RS=1; RW=0;
    PD->DOUT = c;
    delay( 10 ); E=1; delay( 10 ); E=0;
    delay( 100 );
}
#define LCD_ADDR(x)    lcd_cmd( 0x80 | x )
void lcd_print(uint8_t pos, char *s){
    //--- ????
    lcd_cmd(0x80 | pos);
    //--- ???
    while(*s){ lcd_put(*s++); }
}
 
//---
void lcd_init(void){
    int i;
    //--- GPIO
    PB->PMD |= 0x40000000;
    PC->PMD |= 0x50000000;
    PD->PMD |= 0x00005555;
    PD->DMASK |= 0x0000FF00;
    RS=0; RW=0; E=0;
    //--- example
    for(i=0;i<1000000;i++){} //step 1
    lcd_cmd(0x38);      //step 2
    lcd_cmd(0x0C);      //step 3
    lcd_cmd(0x06);      //step 4
    lcd_cmd(0x80);      //step 4.5
    //---
    lcd_print(0x00, "                ");
    lcd_print(0x40, "                ");
}

//-------------- HCLK  -------------
void init_HCLK(void){
  SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HXT_EN_Msk);
    CLK_WaitClockReady( CLK_CLKSTATUS_HXT_STB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT,CLK_HCLK_CLK_DIVIDER(1));
  SYS_LockReg();
}
//--------------- Systick ------------------
volatile uint32_t tick=0;
void SysTick_Handler(void){
    tick++;
};
 
void delay_ms(uint32_t ms){
    ms += tick;
    while( ms != tick){}
}
//----------------- TIMER0 --------------------
void init_TIMER0(int freq){
  SYS_UnlockReg();
    //--- CLK
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, 0);
    //--- PIN
  SYS_LockReg();
    //--- OPEN
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, freq);
    TIMER_Start(TIMER0);
    //--- NVIC
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
}
    //--- ISR
void TMR0_IRQHandler(void){
		PA13 ^= 1;
    TIMER_ClearIntFlag( TIMER0 );
}
//----------------- TIMER1 --------------------
void init_TIMER1(int freq){
  SYS_UnlockReg();
    //--- CLK
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HXT, 0);
    //--- PIN
    SYS->PB_H_MFP &= ~SYS_PB_H_MFP_PB9_MFP_Msk;
    SYS->PB_H_MFP |= SYS_PB_H_MFP_PB9_MFP_TMR1_EXT;
  SYS_LockReg();
    //--- OPEN
    TIMER_Open(TIMER1, TIMER_TOGGLE_MODE, freq);
    TIMER_Start(TIMER1);
    //--- NVIC
    TIMER_EnableInt(TIMER1);
    //NVIC_EnableIRQ(TMR1_IRQn);
}
    //--- ISR
void TMR1_IRQHandler(void){
    TIMER_ClearIntFlag( TIMER1 );
}
//---- INIT
void init(void){
	init_HCLK();
    SysTick_Config( SystemCoreClock /1000);
	//---
	lcd_init();
	lcd_print(0x40, "hello");
	//--- PA12
	GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);
	//--- TIMER0
	init_TIMER0(10);
	init_TIMER1(2000);
}
//---- MAIN
int cnt=0;
int main(void){
	init();
	//---
	while(1){
		char b[64];
		delay_ms(1000);
		cnt++;
		sprintf(b, "%4d", cnt);
		lcd_print(0, b);
	}
}






