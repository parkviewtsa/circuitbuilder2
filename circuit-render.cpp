#include <circuit-render.h>



crIndex crLastError = 0;
crIndex crGetError ()
{
	crIndex err = crLastError;
	crLastError = 0;
	return err;
};



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



crProto** crProtos = NULL;
crIndex crProtoCount = 0;
crProto* crRequireProto (char* path) /// If NULL, something went wrong.
{
	if (!path)
	{
		crLastError = crError_NoProtoPath;
		return NULL;
	};
	crIndex i;
	crProto* item;
	for (i = 0; i < crProtoCount; i++)
	{
		item = *(crProtos + i);
		if (!strcmp(item->loadedfrom,path)) return item; // Already exists.
	};
	FILE* file = fopen(path,"r");
	if (!file)
	{
		crLastError = crError_NoProtoFile;
		return NULL;
	};
	long unsigned linecount;
	// Get number of lines.
	if (!fscanf(file,"%lu",&linecount))
	{
		crLastError = crError_BadProtoFile;
		fclose(file); // Let it go. Else the file will be frozen until the app exits.
		return NULL;
	};
	// Get actual lines.
	crLine* lines = malloc(sizeof(crLine) * linecount);
	if (!lines)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		fclose(file); // Let it go. Else the file will be frozen until the app exits.
		return NULL;
	};
	item = malloc(sizeof(crProto));
	if (!item)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		fclose(file); // Let it go. Else the file will be frozen until the app exits.
		return NULL;
	};
	item->linecount = linecount;
	item->lines = lines;
	item->loadedfrom = path;
	for (i; i < linecount; i++)
	{
		double x1,y1,x2,y2;
		if (fscanf(file,"%lf%lf%lf%lf",&x1,&y1,&x2,&y2) < 4)
		{
			crLastError = crError_BadProtoFile;
			fclose(file); // Let it go. Else the file will be frozen until the app exits.
			free(item); // Let's not have a memory leak on our hands.
			return NULL;
		};
		lines->x1 = x1;
		lines->y1 = y1;
		lines->x2 = x2;
		lines->y2 = y2;
		lines++;
	};
	fclose(file);
	// Now add this to the archive.
	crProtos = realloc(crProtos,sizeof(void*) * (crProtoCount + 1));
	if (!crProtos)
	{
		// You done screwed it up, now. *cough* thanks a lot windows *cough*
		printf("Fatal allocation failure: crProtos main list block realloc\n");
		exit(-1);
	};
	*(crProtos + crProtoCount) = item;
	crProtoCount++;
	return item;
};



SDL_Window* crWindow = NULL;
SDL_Renderer* crRenderer = NULL;
crScalar crViewZoom = 0.1;
crScalar crViewOffsetX = -0.1;
crScalar crViewOffsetY = -0.8;
bool crInit () // true: failure, false: success
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		crLastError = crError_SDL_InitFailure;
		return true;
	};
	crWindow = SDL_CreateWindow("CircuitBuilder2 - Internal Rendering Window",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1280,720,
		#ifdef crShowWindow
		SDL_WINDOW_RESIZABLE
		#else
		SDL_WINDOW_HIDDEN
		#endif
	);
	if (!crWindow)
	{
		crLastError = crError_SDL_NoWindow;
		SDL_Quit();
		return true;
	};
	crRenderer = SDL_CreateRenderer(crWindow,-1,0);
	if (!crRenderer)
	{
		crLastError = crError_SDL_NoRenderer;
		SDL_DestroyWindow(crWindow);
		SDL_Quit();
		return true;
	};
	return false;
};
void crDraw ()
{
	int winsizex,winsizey;
	SDL_GetWindowSize(crWindow,&winsizex,&winsizey);
	SDL_SetRenderDrawColor(crRenderer,0,0,0,0);
	SDL_RenderClear(crRenderer);
	SDL_SetRenderDrawColor(crRenderer,255,255,255,255);
	for (crIndex i = 0; i < crItemCount; i++)
	{
		crItem* item = *(crItems + i);
		for (crIndex j = 0; j < item->proto->linecount; j++)
		{
			crLine line = *(item->proto->lines + j);
			line.x1 += item->posx - crViewOffsetX;
			line.y1 += item->posy - crViewOffsetY;
			line.x2 += item->posx - crViewOffsetX;
			line.y2 += item->posy - crViewOffsetY;
			SDL_RenderDrawLine(
				crRenderer,
				crDrawRound((line.x1 * crViewZoom) * winsizex),
				crDrawRound((line.y1 * crViewZoom) * winsizex),
				crDrawRound((line.x2 * crViewZoom) * winsizex),
				crDrawRound((line.y2 * crViewZoom) * winsizex)
			);
			// Render coords are not normalized.
			// This is using winsizex only, because using
			// winsizex and winsizey will not preserve aspect ratio.
		};
	};
	SDL_RenderPresent(crRenderer);
};
void crQuit ()
{
	SDL_DestroyRenderer(crRenderer);
	SDL_DestroyWindow(crWindow);
	SDL_Quit();
};
