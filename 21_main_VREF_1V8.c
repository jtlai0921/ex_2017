#include "Nano100Series.h"              // Device header
#include <stdio.h>

void lcd_print(uint8_t pos, char *s);
void lcd_init(void);

#define QA  PE7
#define QB  PE8
#define QC  PE9
#define QD  PE10

int m0=0, m1=0;
void motor(void){	
		GPIO_SetMode(PE, BIT7+BIT8+BIT9+BIT10, GPIO_PMD_OUTPUT);
	//---
		QA=QB=QC=QD=0;
	if(m0==m1){ return; }
	if(m0>m1){ m0--; }
	if(m0<m1){ m0++; }
	//---
	switch(m0&3){
		case 0: QA=1; break;
		case 1: QB=1; break;
		case 2: QC=1; break;
		case 3: QD=1; break;
	}
}
void motor_set(int n){
	m1 = n;
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
	motor();
    TIMER_ClearIntFlag( TIMER0 );
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
uint32_t tick=0;
void SysTick_Handler(void){
    tick++;
};
 
void delay_ms(uint32_t ms){
    ms += tick;
    while( ms != tick){}
}
//------------------- CAPTURE ---------------------
void init_PWM1_cap(void){
  SYS_UnlockReg();
    //--- CLK
    CLK_EnableModuleClock(PWM1_CH23_MODULE);
    CLK_SetModuleClock(PWM1_CH23_MODULE, CLK_CLKSEL2_PWM1_CH23_S_HXT, 0);
    //--- PIN
    SYS->PE_L_MFP &= ~SYS_PE_L_MFP_PE1_MFP_Msk;
    SYS->PE_L_MFP |=  SYS_PE_L_MFP_PE1_MFP_PWM1_CH3;
  SYS_LockReg();
    //--- OPEN
    PWM_ConfigCaptureChannel( PWM1, 3, 50, 0);
    PWM_EnableCapture(PWM1, BIT3);
    PWM_SET_PRESCALER(PWM1, 3, 12 );
    PWM_Start(PWM1, BIT3);
    //--- NVIC
    PWM_EnableCaptureInt(PWM1, 3, PWM_FALLING_LATCH_INT_ENABLE);
    NVIC_EnableIRQ(PWM1_IRQn);
}
uint16_t Capi=0,Cap0,Cap1,Cap2;
uint32_t code=0;
uint16_t IRcode=0;
char IRF=0;

    //--- ISR
void PWM1_IRQHandler(void){
	
    Cap0 = Cap1;
    Cap1 = PWM1->CFL3;
    Cap2 = Cap0 - Cap1;
    if((Cap2 < 500)||(Cap2 > 2500)){ Capi=0; }
    else{
        code <<=1;
        if(Cap2>1500){ code++ ; }
        Capi++;
        if(Capi==32){ IRcode = code; IRF=1; }
    }
		
    PWM_ClearCaptureIntFlag(PWM1, 3, PWM_FALLING_LATCH_INT_FLAG);

}
//---
void IR_proc(int n){
	switch(n){
		case 0x30CF: PA12 ^=1; break;
		case 0x18E7: PA13 ^=1; break;
		case 0x7A85: PA14 ^=1; break;
	}
}
void IR_proc_0(int n){
	switch (n){
		case 0x30CF: PA12=1; PA13=PA14=0; break;
		case 0x18E7: PA13=1; PA12=PA14=0; break;
		case 0x7A85: PA14=1; PA12=PA13=0; break;
	}
}
//------------------ ADC ---------------------
void init_ADC(void){
  SYS_UnlockReg();
    //--- ADC CLK
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HXT, CLK_ADC_CLK_DIVIDER(12));
    //--- ADC PIN
    SYS->PA_L_MFP &= ~SYS_PA_L_MFP_PA0_MFP_Msk;
    SYS->PA_L_MFP |=  SYS_PA_L_MFP_PA0_MFP_ADC_CH0;
    SYS->PA_L_MFP &= ~SYS_PA_L_MFP_PA1_MFP_Msk;
    SYS->PA_L_MFP |=  SYS_PA_L_MFP_PA1_MFP_ADC_CH1;
 
    GPIO_DISABLE_DIGITAL_PATH( PA, (BIT0+BIT1));
  SYS_LockReg();
    //--- ADC OPEN
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE_CYCLE, BIT0+BIT1+BIT15 );
    ADC_POWER_ON();
    //--- ADC NVIC
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}
//--- ADC ISR
void ADC_IRQHandler(void){
	char b[32];
	int c;
	sprintf(b, "%4d", ADC->RESULT[0]&0xFFF);
	lcd_print(0x40, b);
	//---
	c = 180*4095/(ADC->RESULT[15]&0xFFF);
	sprintf(b, "V: %d.%02d", c/100, c%100);
	lcd_print(0x48, b);
	//---
		motor_set( ADC->RESULT[0]&0xFFF );
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
}
//--------------- VREF -----------------
// SYS->Int_VREFCTL
// ????
#define _EXT_MODE   (1<<3)
#define _SEL25      (1<<2)
#define _REG_EN     (1<<1)
#define _BGP_EN     (1<<0)
//-- ????
#define _VREF_1V8   (_EXT_MODE+_REG_EN+_BGP_EN)
#define _VREF_2V5   (_EXT_MODE+_SEL25+_REG_EN+_BGP_EN)
#define _VREF_OFF   0
 
#define VREF_OFF    0
#define VREF_1V8    18
#define VREF_2V5    25
 
void CFG_VREF(int n){
  SYS_UnlockReg();
    switch (n){
        case 0:  SYS->Int_VREFCTL = _VREF_OFF; break;
        case 18: SYS->Int_VREFCTL = _VREF_1V8; break;
        case 25: SYS->Int_VREFCTL = _VREF_2V5; break;
    }
  SYS_LockReg();
}
 
//----- INIT
void init(void){
	init_HCLK();
    SysTick_Config( SystemCoreClock /1000);
	//--- PA12
	GPIO_SetMode(PA, BIT12+BIT13+BIT14, GPIO_PMD_OUTPUT);
	//--- PE1
	init_PWM1_cap();
	//--- TIMER0
	init_TIMER0(250);
	//--- ADC
	init_ADC();
	//--- LCD
	lcd_init();
	lcd_print(5, "Hello");
	//--- VREF
	CFG_VREF(VREF_1V8);
}
//--- MAIN
int main(void){
		init();
	
	while(1){
		delay_ms(100);
		ADC_START_CONV(ADC);
	}
	
	
		while(1){
			delay_ms(500);
			if(IRF){
				IR_proc(IRcode);
				IRF=0;
			}
		}
}
