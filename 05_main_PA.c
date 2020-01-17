#include "Nano100Series.h"              // Device header


int main(void){
	PA->PMD = 0x01000000;
	PA->DOUT = 0x00001000;
	//--- SW2
	PB->PMD = 0x00000000;
	PB->PUEN |= (1<<2);
	
	while(1){
		PA12 = ~PB2;
	}
}
