#include "Nano100Series.h"              // Device header

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

void clk_test(void){
	init_HCLK();
  SYS_UnlockReg();
		CLK_EnableCKO(CLK_CLKSEL2_FRQDIV_S_HXT, 2);
		SYS->PB_H_MFP &= ~SYS_PB_H_MFP_PB12_MFP_Msk;
		SYS->PB_H_MFP |= SYS_PB_H_MFP_PB12_MFP_CKO;
	SYS_LockReg();	
}
//---
void SysTick_Handler(void){
	PA12 ^= 1;
}
//---- MAIN
uint32_t sclk;
int main(void){
	clk_test();
	//---
	GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);
	sclk = SystemCoreClock;
	SysTick_Config(SystemCoreClock /1000);
	//---
	lcd_init();
	lcd_print(5, "Hello");
	//---
	while(1){
	}
}
