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
	crWindow = SDL_CreateWindow("CircuitBuilder2 - Internal Rendering Window",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1280,720,SDL_WINDOW_HIDDEN);
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
