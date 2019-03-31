/*
	whoami与iam系统调用的实现
	by Focus2019
*/

#include <string.h>
#include <errno.h>
#include <asm/segment.h>

char username[24];

int sys_iam(const char *name)
{
	
	int i;
	char tmp[24];
	for(i = 0; i < 24; i++)
		tmp[i] = '\0';
	for(i = 0; i < 24; i++)
	{
		tmp[i] = get_fs_byte(name + i);
		if(tmp[i] == '\0') break;
	}
	if(tmp[23] != '\0')
	{
		return -(EINVAL);
	}
	strcpy(username, tmp);
	return i;
}

int sys_whoami(char *name, unsigned int size)
{
	int insize = strlen(username);
	if(size < insize)
		return -(EINVAL);
	int i;
	for(i = 0; i < size; i++)
	{
		put_fs_byte(username[i], name+i);
		if(username[i] == '\0') break;
	}
	return i;
}
