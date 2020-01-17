#include "Nano100Series.h"              // Device header

int main(void){
	*(volatile unsigned int *)0x50004000 = 0x01000000;
	*(volatile unsigned int *)0x50004008 = 0x00001000;
	
	while(1){
		int i;
		for(i=0;i<1000000;i++){}
			*(volatile unsigned int *)0x50004008 ^= 0x00001000;
	}
}
