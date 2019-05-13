#include<stdio.h>
#include<stdlib.h>
#include"bigint.h"


int main(void)
{	
	BigInt n,t,soma,g;
	int i;
	bi_init(128);		
	n = bi_new (5318973);
	t = bi_new (-21643753);
	g = bi_new (-2004318072);
	soma = bi_mul(g,t);	
	for(i=0;i<16;i++)
		printf("valor:%02x\n",soma[i]);
	return 0;
}