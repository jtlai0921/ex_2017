#include "Nano100Series.h"              // Device header

#define	SFR32(a)	(*(volatile unsigned int *)a)
#define	GPIOA_PMD		SFR32(0x50004000)
#define	GPIOA_DOUT	SFR32(0x50004008)
typedef volatile struct{
	volatile unsigned long PMD;
	volatile unsigned long OFFD;
	volatile unsigned long DOUT;
	volatile unsigned long DMASK;
	volatile unsigned long PIN;
	volatile unsigned long DBEN;
	volatile unsigned long IMD;
	volatile unsigned long IER;
	volatile unsigned long ISRC;
	volatile unsigned long PUEN;	
}_TPGPIO;

#define	GPIOA	((_TPGPIO*)0x50004000)
#define	GPIOB	((_TPGPIO*)0x50004040)
int main(void){
	GPIOA->PMD = 0x01000000;
	GPIOA->DOUT = 0x00001000;
	GPIOB->PMD = 0x01000000;
	
	while(1){
		int i;
		for(i=0;i<1000000;i++){}
			GPIOA_DOUT ^= 0x00001000;
	}
}
