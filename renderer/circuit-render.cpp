#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <math.h>

#define crShowWindow
#include "circuit-render.h"
#define CB_GROW_ALLOC_FAIL 1984

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
	out->scale = 1;
	crItems = (crItem**)realloc(crItems,sizeof(void*) * (crItemCount + 1));
	if (!crItems)
	{
		// You done screwed it up, now. *cough* thanks a lot windows *cough*
		printf("Fatal allocation failure: crItems main list block realloc, during grow\n");
		exit(CB_GROW_ALLOC_FAIL);
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
                // no chance of recovery from this
				printf("Fatal allocation failure: crItems main list block realloc, during shrink\n");
				exit(CB_GROW_ALLOC_FAIL);
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
		goto FAIL_5;
	};
	// Get number of lines.
	long unsigned linecount;
	if (!fscanf(file,"%lu",&linecount))
	{
		crLastError = crError_BadProtoFile;
		goto FAIL_4;
	};
	// Get actual lines.
	crLine* lines = (crLine*)malloc(sizeof(crLine) * linecount);
	if (!lines)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		goto FAIL_4;
	};
	// Get number of circles.
	crIndex circlecount;
	if (!fscanf(file,"%lu",&circlecount))
    {
        crLastError = crError_BadProtoFile;
        goto FAIL_3;
    };
    // Get actual circles.
    crCircle* circles = (crCircle*)malloc(sizeof(crCircle) * circlecount);
    if (!circles)
    {
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		goto FAIL_3;
    };
	item = (crProto*)malloc(sizeof(crProto));
	if (!item)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		goto FAIL_2;
	};
	size_t pathlen = strlen(path) + 1; // Include the NULL character.
	item->loadedfrom = malloc(pathlen); // Keep an internal copy.
	if (!item->loadedfrom)
	{
		// That memory was really important to me.
		goto FAIL_1;
	};
	memcpy(item->loadedfrom,path,pathlen); // Copy string.
	item->linecount = linecount;
	item->lines = lines;
	item->circlecount = circlecount;
	item->circles = circles;
	item->loadedfrom = path;
	printf("Reached\n");
	for (i = 0; i < linecount; i++)
	{
		double x1,y1,x2,y2;
		if (fscanf(file,"%lf%lf%lf%lf",&x1,&y1,&x2,&y2) < 4)
		{
			crLastError = crError_BadProtoFile;
			goto FAIL_0;
		};
		lines->x1 = x1;
		lines->y1 = y1;
		lines->x2 = x2;
		lines->y2 = y2;
		lines++;
	};
	for (i = 0; i < circlecount; i++)
    {
        double x,y,r;
        if (fscanf(file,"%lf%lf%lf",&x,&y,&r) < 3)
        {
            crLastError = crError_BadProtoFile;
			goto FAIL_0;
        };
		circles->x = x;
		circles->y = y;
		circles->r = r;
		circles++;
    };
	fclose(file);
	// Now add this to the cache.
	crProtos = (crProto**)realloc(crProtos,sizeof(void*) * (crProtoCount + 1));
	if (!crProtos)
	{
		// no chance of recovery from this
		printf("Fatal allocation failure: crProtos main list block realloc\n");
		exit(CB_GROW_ALLOC_FAIL);
	};
	*(crProtos + crProtoCount) = item;
	crProtoCount++;
	return item;
	// Cleaning up after errors (this section not reached if nothing went wrong).
	FAIL_0:
    free(item->loadedfrom); // Avoid a memory leak.
    FAIL_1:
    free(item); // Avoid a memory leak.
    FAIL_2:
    free(circles); // Avoid a memory leak.
    FAIL_3:
    free(lines); // Avoid a memory leak.
    FAIL_4:
    fclose(file); // Let it go. Elsa the file will be frozen until the app exits.
    FAIL_5:
    return NULL;
};



SDL_Window* crWindow = NULL;
SDL_Renderer* crRenderer = NULL;
crScalar crViewW = 10;
crScalar crViewX = 0;
crScalar crViewY = 0;
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
SDL_Texture* crDrawCircle (crIndex radius, SDL_Color color)
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
    #else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
    #endif
    crIndex surfdims = (radius * 2) + 1;
    SDL_Surface* surf = SDL_CreateRGBSurface(0,surfdims,surfdims,32,rmask,gmask,bmask,amask);
    signed long r_sq = radius * radius;
    Uint8* pixel = surf->pixels;
    for (crIndex y = 0; y < surfdims; y++)
    {
        crIndex y_sq = y - (radius + 1);
        y_sq *= y_sq;
        for (crIndex x = 0; x < surfdims; x++)
        {
            crIndex x_sq = x - (radius + 1);
            x_sq *= x_sq;
            SDL_Color writecolor = {0,0,0,0};
            signed long total = y_sq + x_sq;
            if (total - r_sq < 0) if (powf(total,0.5f) - radius >= -1) writecolor = color;
            *pixel = writecolor.r;
            pixel++;
            *pixel = writecolor.g;
            pixel++;
            *pixel = writecolor.b;
            pixel++;
            *pixel = writecolor.a;
            pixel++;
        };
    };
    SDL_Texture* tex = SDL_CreateTextureFromSurface(crRenderer,surf);
    SDL_FreeSurface(surf);
    return tex;
};
void crDraw ()
{
	if (!crReady) if (crInit()) return; // Can't use SDL stuff unless it is initialized.
	int winsizex,winsizey;
	SDL_GetWindowSize(crWindow,&winsizex,&winsizey);
    crScalar viewh = crViewW / (winsizex / (crScalar)winsizey);
	SDL_SetRenderDrawColor(crRenderer,0,0,0,0);
	SDL_RenderClear(crRenderer);
	for (crIndex i = 0; i < crItemCount; i++)
	{
		crItem* item = *(crItems + i);
		SDL_SetRenderDrawColor(crRenderer,item->color.r,item->color.g,item->color.b,item->color.a);
		crIndex j;
		for (j = 0; j < item->proto->linecount; j++)
		{
			crLine line = *(item->proto->lines + j);
			line.x1 *= item->scale;
			line.y1 *= item->scale;
			line.x2 *= item->scale;
			line.y2 *= item->scale;
			line.x1 += item->posx;
			line.y1 += item->posy;
			line.x2 += item->posx;
			line.y2 += item->posy;
			line.x1 -= crViewX;
			line.y1 -= crViewY;
			line.x2 -= crViewX;
			line.y2 -= crViewY;
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
		for (j = 0; j < item->proto->circlecount; j++)
        {
            crCircle circle = *(item->proto->circles + j);
            SDL_Rect rect;
            rect.w = ((circle.r * item->scale) / crViewW) * winsizex;
            SDL_Texture* tex = crDrawCircle(rect.w,item->color);
            rect.w *= 2;
            rect.w++;
            rect.h = rect.w;
            rect.x = ((((circle.x * item->scale) + item->posx) - crViewX) / crViewW) + 0.5;
            rect.y = ((((circle.x * item->scale) + item->posx) - crViewX) / crViewW) + 0.5;
            SDL_RenderCopy(crRenderer,tex,NULL,&rect);
            SDL_DestroyTexture(tex);
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
	// Give this function coordinates in range ([0,1],[0,1]).
	x -= 0.5;
	x *= crViewW;
	x += crViewX;
	y -= 0.5;
	int winsizex,winsizey;
	SDL_GetWindowSize(crWindow,&winsizex,&winsizey);
	crScalar viewh = crViewW / (winsizex / (crScalar)winsizey);
	y *= viewh;
	y += crViewY;
	crItem* out = NULL;
	crScalar closest;
	for (crIndex cur = 0; cur < crItemCount; cur++)
	{
		crItem* item = *(crItems + cur);
		crScalar x_sq = item->posx - x;
		x_sq *= x_sq;
		crScalar y_sq = item->posy - y;
		y_sq *= y_sq;
		x_sq += y_sq; // Reuse of this variable.
		if (x_sq <= 1)
		{
			// The radius of the circle is 1. 1 squared is 1.
			if (out) if (x_sq > closest) continue; // Not as close as another. Skip!
			// Otherwise, we have a winner here.
			out = item;
			closest = x_sq;
		};
	};
	return out; // Return NULL if nothing at least nearby.
};

void cbWorldToScreen (crScalar* x, crScalar* y)
{
    if (x) *x = ((*x - crViewX) / crViewW) + 0.5;
    if (y)
    {
        int winx,winy;
        SDL_GetWindowSize(crWindow,&winx,&winy);
        crScalar viewh = crViewW / (winx / (crScalar)winy);
        *y = ((*y - crViewY) / viewh) + 0.5;
    };
};

void cbScreenToWorld (crScalar* x, crScalar* y)
{
    if (x) *x = ((*x - 0.5) * crViewW) + crViewX;
    if (y)
    {
        int winx,winy;
        SDL_GetWindowSize(crWindow,&winx,&winy);
        crScalar viewh = crViewW / (winx / (crScalar)winy);
        *y = ((*y - 0.5) * viewh) + crViewY;
    };
};
