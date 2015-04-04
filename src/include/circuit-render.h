#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef crUseDoubles
typedef double crScalar;
#else
typedef float crScalar;
#endif
#ifdef crUseLongs
typedef unsigned long crIndex;
#else
typedef unsigned int crIndex;
#endif
// I've no idea where this file went
//#include "circuit-render-error.h"
#include "circuit-render-proto.h"

struct crItem
{
	crProto* proto;
	crScalar posx,posy;
};

crItem** crItems = NULL;
crIndex crItemCount = 0;
crItem* crCreateItem (char* path);
