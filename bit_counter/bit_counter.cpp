//
//	byte_counter.cpp counts number of binary 1s
//	in a bit string converted from a given hex value
//
//	To Compile:	gcc -o byte_counter byte_counter.cpp -lpthread
//

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <regex.h>

#define BIT0 0x01

//	Define the pattern to verify hex values
#define HEX_PATTERN "0x([A-F]|[a-f]|[0-9])+"

//	validate input value
int match (const char *string)
{
	regex_t re;
	if (regcomp (&re, HEX_PATTERN, REG_EXTENDED|REG_NOSUB) != 0) 
	{
		return 0;
	}
	
	int status = regexec (&re, string, 0, NULL, 0);
	regfree(&re);
	if (status != 0)
	{ 
		return 0;
	}
	return 1;
}

//	count the ones in a single hex digit (4 bits)
static int bit_count (char d)
{
	char val  = d;		//	int value of hex digit
	int	count = 0;		//	number of ones 	
	
	while (val > 0)
	{
		if (val & BIT0)
		{
			count++;
		}
		val = val >> 1;
	}
	return count;
}

int main (int argc, char *argv[])
{
    char c = *argv[1];
    printf ("%s_%s: Hex value: 0x%02x has %02d ones.", argv[0], __func__,
			c, bit_count(c));    
}