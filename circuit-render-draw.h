#pragma once
#define SDL_main main
#include <SDL2/SDL.h>
SDL_Window* crWindow = NULL;
SDL_Renderer* crRenderer = NULL;
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
#define crDrawRound(n) (int)(n + 0.5)
crScalar crViewZoom = 0.1;
crScalar crViewOffsetX = -0.1;
crScalar crViewOffsetY = -0.8;
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
