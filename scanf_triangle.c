#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define X_NAME "first"
#define Y_NAME "second"
#define Z_NAME "hipo"

#define MAX_LEN (10)
#define FMT_LEN (20)
#define INT_LEN (5)

int main() 
{
	int x = 0;
	int y = 0;
	int z = 0;
	
	char fmtstr[INT_LEN + FMT_LEN] = {0};
	sprintf(fmtstr, " %%%d[" X_NAME Y_NAME Z_NAME "] = %%d", MAX_LEN);
	
	printf("Input:\n");
	
	int tmpval = 0;
	char tmpstr[MAX_LEN] = {0};
	
	for (int i = 0; i < 3; ++i) {
		if (scanf(fmtstr, tmpstr, &tmpval) < 2) {
			printf("Incorrect input\n");
			exit(1);
		}
		
		if 		(strcmp(tmpstr, X_NAME) == 0)
			x = tmpval;
		else if (strcmp(tmpstr, Y_NAME) == 0)
			y = tmpval;
		else if (strcmp(tmpstr, Z_NAME) == 0)
			z = tmpval;
		else {
			printf("Incorrect input\n");
			exit(1);
		}
	}
	
	if (x * x + y * y == z * z)
		printf("Yes\n");
	else 
		printf("No\n");
}
