static char *rcsId = "$Header: /misc/hadesprojects/daq/cvsroot/eventbuilder/hadaq/hadtu.c,v 6.6 2003-05-14 13:54:43 muench Exp $";


#define _ANSI_C_SOURCE
#include <stddef.h>

#include <stdio.h>
#include <string.h>


#include "hadtu.h"

char *HadTu_2charP(const void *my)
{
	static char buf[80];

	sprintf(buf, "size: 0x%08x\tdecoding: 0x%08x",
			HadTu_size(my), HadTu_decoding(my));

	return buf;
}
