#ifndef _PRINT_HEX_H
#define _PRINT_HEX_H

#include <stdio.h>
#include <stdlib.h>
#include "short_types.h"

//#define DONT_PRINT_0X
//#define REMOVE_0

void print_hex(const char *desc, u8 *s, int size) { //Used for printing keys/other values
	printf("%s", desc);
#ifndef DONT_PRINT_0X
	printf("0x");
#endif
#ifdef REMOVE_0
	int startp = 0;
	int i;
	for (i = 0; i < size; i++) {
		if (startp == 0 && s[i] == 0) //If the value is 0 and we aren't in the middle of the number don't print it
			continue;
		if (startp == 0 && s[i] != 0) { //The value is not 0: print the next value when 0 too
			startp = 1;
			printf("%X", s[i]);
			continue;
		}
		printf("%02X", s[i]);
	}
	if (startp == 0) //It was all 0 so nothing got printed, print 0
		printf("0");
#else
	int i;
	for (i = 0; i < size; i++)
		printf("%02X", s[i]);
#endif
	printf("\n");
}

#endif