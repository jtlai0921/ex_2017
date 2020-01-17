#include "Nano100Series.h"              // Device header

#define	SFR32(a)	(*(volatile unsigned int *)a)
#define	GPIOA_PMD		SFR32(0x50004000)
#define	GPIOA_DOUT	SFR32(0x50004008)

int main(void){
	GPIOA_PMD = 0x01000000;
	GPIOA_DOUT = 0x00001000;
	
	while(1){
		int i;
		for(i=0;i<1000000;i++){}
			GPIOA_DOUT ^= 0x00001000;
	}
}
