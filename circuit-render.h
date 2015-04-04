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
	crProto* proto = crRequireProto(path);
	if (!proto) return NULL; // crLastError will remain from the lower level.
	crItem* out = malloc(sizeof(crItem));
	if (!out)
	{
		crLastError = crError_FailedAlloc;
		return NULL;
	};
	out->proto = proto;
	crItems = realloc(crItems,sizeof(void*) * (crItemCount + 1));
	if (!crItems)
	{
		// You done screwed it up, now. *cough* thanks a lot windows *cough*
		printf("Fatal allocation failure: crItems main list block realloc, during grow\n");
		exit(-1);
	};
	*(crItems + crItemCount) = out;
	crItemCount++;
	return out;
};
#include <circuit-render-draw.h>
void crDestroyItem (crItem* todel)
{
	for (crIndex i = 0; i < crItemCount; i++)
	{
		crItem* item = *(crItems + i);
		if (item == todel)
		{
			crItemCount--;
			while (i < crItemCount)
			{
				*(crItems + i) = *(crItems + (i + 1));
				i++;
			};
			crItems = realloc(crItems,sizeof(void*) * crItemCount);
			if (!crItems)
			{
				// You done screwed it up, now. *cough* thanks a lot windows *cough*
				printf("Fatal allocation failure: crItems main list block realloc, during shrink\n");
				exit(-1);
			};
			break;
		};
	};
	free(todel);
};
