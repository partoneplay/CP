#include <stdio.h>
#include <stdlib.h>
#include "linux.h"

int main(int argc, char **argv)
{
	int a = copyDir(argv[1], argv[2]);
	
	return 0;
}