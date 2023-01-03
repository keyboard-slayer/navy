#include "ctype.h"

int isgraph(int c)
{
	return c >= '!' && c <= '~';
}


int isspace(int c)
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
			c == '\v';
}

int isprint(int c)
{
	return (isspace(c) || isgraph(c));
}

int isdigit(int c)
{
	return c >= '0' && c <= '9';
}