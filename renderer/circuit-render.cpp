#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#define crShowWindow
#include "circuit-render.h"

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
	crItem* out = (crItem*)malloc(sizeof(crItem));
	if (!out)
	{
		crLastError = crError_FailedAlloc;
		return NULL;
	};
	out->proto = proto;
	out->color = {255,255,255,255}; // Default: Solid White
	crItems = (crItem**)realloc(crItems,sizeof(void*) * (crItemCount + 1));
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
			crItems = (crItem**)realloc(crItems,sizeof(void*) * crItemCount);
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
	crLine* lines = (crLine*)malloc(sizeof(crLine) * linecount);
	if (!lines)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		fclose(file); // Let it go. Else the file will be frozen until the app exits.
		return NULL;
	};
	item = (crProto*)malloc(sizeof(crProto));
	if (!item)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		fclose(file); // Let it go. Else the file will be frozen until the app exits.
		free(lines); // Avoid a memory leak.
		return NULL;
	};
	size_t pathlen = strlen(path) + 1; // Include the NULL character.
	item->loadedfrom = malloc(pathlen); // Keep an internal copy.
	if (!item->loadedfrom)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		fclose(file); // Let it go. Else the file will be frozen until the app exits.
		free(lines); // Avoid a memory leak.
		free(item); // Avoid a memory leak.
		return NULL;
	};
	memcpy(item->loadedfrom,path,pathlen); // Copy string.
	item->linecount = linecount;
	item->lines = lines;
	item->loadedfrom = path;
	for (i = 0; i < linecount; i++)
	{
		double x1,y1,x2,y2;
		if (fscanf(file,"%lf%lf%lf%lf",&x1,&y1,&x2,&y2) < 4)
		{
			crLastError = crError_BadProtoFile;
			fclose(file); // Let it go. Else the file will be frozen until the app exits.
			free(lines); // Avoid a memory leak.
			free(item); // Avoid a memory leak.
			free(item->loadedfrom); // Avoid a memory leak.
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
	crProtos = (crProto**)realloc(crProtos,sizeof(void*) * (crProtoCount + 1));
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
crScalar crViewW = 10;
crScalar crViewX = -0.1;
crScalar crViewY = -0.8;
void crGetViewPos (crScalar* x, crScalar* y)
{
	if (x) *x = crViewX;
	if (y) *y = crViewY;
};
void crSetViewPos (crScalar x, crScalar y)
{
	crViewX = x;
	crViewY = y;
};
crScalar crGetViewWidth ()
{
	return crViewW;
};
void crSetViewWidth (crScalar w)
{
	crViewW = w;
};
bool crReady = false;
bool crInit () // true: failure, false: success
{
	if (crReady) return false;
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
	crReady = true;
	return false;
};
#define crDrawRound(n) ((int)(n + 0.5f))
void* crImgBuf = NULL;
crIndex crImgW = 0;
crIndex crImgH = 0;
void crGetImg (void** buf, crIndex* w, crIndex* h)
{
	if (buf && w && h)
	{
		crDraw();
		*buf = crImgBuf;
		*w = crImgW;
		*h = crImgH;
	}
	else crLastError = crError_BadArgs;
};
void* crResImgBuf = NULL;
crIndex crResImgW = 0;
crIndex crResImgH = 0;
void* crGetImg_ForceDims (crIndex w_forced, crIndex h_forced)
{
	crResize(w_forced,h_forced);
	crDraw();
	if (!crImgBuf) return NULL;
	/// In most cases, the below condition should be true,
	/// and we won't need to use any kind of resizing procedures.
	if (crImgW == w_forced && crImgH == h_forced) return crImgBuf;
	else
	{
		/// Otherwise, here's a resize operation
		/// with nearest-pixel sampling
		/// so it's going to look rather ugly
		/// and it's going to take a while too.
		// If you feel like implementing interpolation,
		// then I suppose you could, but it's going to be
		// much, much slower (especially considering it
		// will be executed in serial on the CPU) and
		// completely unnecessary because the resize
		// operation is only here for a very unlikely case.
		crLastError = crError_ForcedResize; // I consider this an issue.
		if (crResImgW != w_forced || crResImgH != h_forced)
		{
			if (crResImgBuf) free(crResImgBuf);
			crResImgBuf = malloc(w_forced * h_forced * 3);
			if (!crResImgBuf)
			{
				w_forced = 0;
				h_forced = 0;
				crLastError = crError_FailedAlloc;
				return NULL;
			};
			crResImgW = w_forced;
			crResImgH = h_forced;
		};
		Uint8* resimgptr = crResImgBuf;
		for (crIndex y = 0; y < h_forced; y++)
		{
			crIndex pix_y = (y / (float)h_forced) * crImgH;
			if (pix_y >= crImgH) pix_y = crImgH - 1; // Avoid memory access violations.
			for (crIndex x = 0; x < w_forced; x++)
			{
				crIndex pix_x = (x / (float)w_forced) * crImgW;
				if (pix_x >= crImgW) pix_x = crImgW - 1; // Avoid memory access violations.
				Uint8* color = crImgBuf + (((pix_y * crImgW) + pix_x) * 3);
				*resimgptr = *color;
				resimgptr++;
				color++;
				*resimgptr = *color;
				resimgptr++;
				color++;
				*resimgptr = *color;
				resimgptr++;
				// Don't need to increment `color`.
			};
		};
		return crResImgBuf;
	};
};
void crResize (crIndex w, crIndex h)
{
	int winsizex,winsizey;
	SDL_GetWindowSize(crWindow,&winsizex,&winsizey);
	if (winsizex != w || winsizey != h) SDL_SetWindowSize(crWindow,w,h);
};
void crDraw ()
{
	if (!crReady) if (crInit()) return; // Can't use SDL stuff unless it is initialized.
	int winsizex,winsizey;
	SDL_GetWindowSize(crWindow,&winsizex,&winsizey);
	SDL_SetRenderDrawColor(crRenderer,0,0,0,0);
	SDL_RenderClear(crRenderer);
	for (crIndex i = 0; i < crItemCount; i++)
	{
		crItem* item = *(crItems + i);
		SDL_SetRenderDrawColor(crRenderer,item->color.r,item->color.g,item->color.b,item->color.a);
		for (crIndex j = 0; j < item->proto->linecount; j++)
		{
			crLine line = *(item->proto->lines + j);
			line.x1 += item->posx;
			line.y1 += item->posy;
			line.x2 += item->posx;
			line.y2 += item->posy;
			line.x1 -= crViewX;
			line.y1 -= crViewY;
			line.x2 -= crViewX;
			line.y2 -= crViewY;
			crScalar viewh = crViewW / (winsizex / (crScalar)winsizey);
			line.x1 /= crViewW;
			line.y1 /= viewh;
			line.x2 /= crViewW;
			line.y2 /= viewh;
			line.x1 += 0.5;
			line.y1 += 0.5;
			line.x2 += 0.5;
			line.y2 += 0.5;
			SDL_RenderDrawLine(
				crRenderer,
				crDrawRound(line.x1 * winsizex),
				crDrawRound(line.y1 * winsizey),
				crDrawRound(line.x2 * winsizex),
				crDrawRound(line.y2 * winsizey)
			);
			// Render coords are not normalized.
			// This is using winsizex only, because using
			// winsizex and winsizey will not preserve aspect ratio.
		};
	};
	SDL_RenderPresent(crRenderer);
	if (crImgW != winsizex || crImgH != winsizey)
	{
		if (crImgW == winsizey && crImgH == winsizex) goto NOALLOC;
		if (crImgBuf) free(crImgBuf);
		crImgBuf = malloc(winsizex * winsizey * 3);
		if (!crImgBuf)
		{
			crLastError = crError_FailedAlloc;
			winsizex = 0;
			winsizey = 0;
			return; // Do not attempt the pixel read operation!
		};
		NOALLOC:
		crImgW = winsizex;
		crImgH = winsizey;
	};
	SDL_RenderReadPixels(crRenderer,NULL,SDL_PIXELFORMAT_RGB888,crImgBuf,winsizex * 3);
};
void crQuit ()
{
	if (crReady)
	{
		SDL_DestroyRenderer(crRenderer);
		SDL_DestroyWindow(crWindow);
		SDL_Quit();
		crReady = false;
		if (crImgBuf)
		{
			free(crImgBuf);
			crImgBuf = NULL;
			crImgW = 0;
			crImgH = 0;
		};
		if (crResImgBuf)
		{
			free(crResImgBuf);
			crResImgBuf = NULL;
			crResImgW = 0;
			crResImgH = 0;
		};
	};
};
void crDropAll ()
{
	while (crItemCount) crDestroyItem(*crItems); // Clear all of the items first.
	for (crIndex i = 0; i < crProtoCount; i++)
	{
		crProto* proto = *(crProtos + i);
		free(proto->lines);
		free(proto->loadedfrom);
		free(proto);
	};
	free(crProtos);
	crProtoCount = 0;
};



crItem* crGetClickedItem (crScalar x, crScalar y)
{

};
