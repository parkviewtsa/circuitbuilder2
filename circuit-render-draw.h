#pragma once
//#define SDL_main main
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
#define crRound(n) (int)(n + 0.5)
void crDraw ()
{
	printf("CYCLE START\n");
	SDL_SetRenderDrawColor(crRenderer,0,0,0,0);
	SDL_RenderClear(crRenderer);
	SDL_SetRenderDrawColor(crRenderer,255,255,255,255);
	for (crIndex i = 0; i < crItemCount; i++)
	{
		printf("\tDRAWING ITEM #%d OUT OF %d\n",i + 1,crItemCount);
		crItem* item = *(crItems + i);
		for (crIndex j = 0; j < item->proto->linecount; j++)
		{
			printf("\t\tDRAWING LINE #%d OUT OF %d\n",j + 1,item->proto->linecount);
			crLine line = *(item->proto->lines + j);
			SDL_RenderDrawLine(crRenderer,crRound(line.x1 * 1280),crRound(line.y1 * 720),crRound(line.x2 * 1280),crRound(line.y2 * 720)); // Render coords are not normalized.
			printf("\t\tx1: %d\ty1: %d\tx2: %d\ty2: %d\n",crRound(line.x1 * 1280),crRound(line.y1 * 720),crRound(line.x2 * 1280),crRound(line.y2 * 720));
		};
	};
	printf("PRESENTING\n\n");
	SDL_RenderPresent(crRenderer);
};
