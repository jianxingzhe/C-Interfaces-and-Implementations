#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int main(){
	int i;
	unsigned long j;
	srand((unsigned long)time(0));
	for(i=0;i<256;i++) {
		j=(unsigned long)(rand()%(ULONG_MAX));
		printf("%ld, ",j);
		if((i+1)%6==0)
			printf("\n");
	}
	return 0;
}

