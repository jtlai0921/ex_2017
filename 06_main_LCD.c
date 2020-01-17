#include "Nano100Series.h"              // Device header

void delay(int n){
	int i;
	for(i=0;i<n;i++){}
}

int main(void){
	PA->PMD = 0x01000000;
	PA->DOUT = 0x00001000;
	//--- LCD
	PB->PMD = 0x40000000;
	PC->PMD = 0x50000000;
	PD->PMD = 0x00005555;
	PB15=0;
	//--- STEP1
	delay(1000000);
	//--- STEP2
	PC14=0; PC15=0;
	PD->DOUT = 0x38;
	delay(0); PB15=1;
	delay(0); PB15=0;
	delay(500);
	//--- STEP3
	PC14=0; PC15=0;
	PD->DOUT = 0x0E;
	delay(0); PB15=1;
	delay(0); PB15=0;
	delay(500);
	//--- STEP4	
	PC14=0; PC15=0;
	PD->DOUT = 0x06;
	delay(0); PB15=1;
	delay(0); PB15=0;
	delay(500);
	//--- STEP5
	PC14=0; PC15=1;
	PD->DOUT = 0x48;
	delay(0); PB15=1;
	delay(0); PB15=0;
	delay(500);
	
	
	
	
	while(1){
		PA12 = ~PB2;
	}
}
