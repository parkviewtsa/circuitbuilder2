#pragma once
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
#include <stdlib.h>
#include <stdio.h>
#include <circuit-render-error.h>
#include <circuit-render-proto.h>
struct crItem
{
	crProto* proto;
	crScalar posx,posy;
};
/// The reason crItems is not a single-block list is
/// so that expanding the list of items does not break
/// the application's references to previously created items.
crItem** crItems = NULL;
crIndex crItemCount = 0;
crItem* crCreateItem (char* path) // Returns NULL if something went wrong.
{
	if (crRequireProto(path)) return; // crLastError will remain from the lower level.
	crItem
};
