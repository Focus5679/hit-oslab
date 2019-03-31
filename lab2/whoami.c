/*
 *  linux/lib/iam.c
 *
 *  (C) 2019  Focus
 */
#include <errno.h>
#define __LIBRARY__
#include <stdio.h>
#include <unistd.h>

_syscall2(int,whoami,char *,name,unsigned int, size)
int main()
{
	char s[30];
	whoami(s, 30);
	printf("%s\n",s);
	return 0;
}

