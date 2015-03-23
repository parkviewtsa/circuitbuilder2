#include <SDL2\SDL.h>
#include <stdlib.h>
typedef unsigned long cbIndex;
typedef double cbScalar;
struct cbVec2
{
	cbScalar x,y;
};
#include <menu.h>
struct cbLD_Circle
{
	cbScalar radius;
	cbVec2 center;
	bool filled;
};
struct cbLD_LineSeg
{
	cbVec2 from,to;
};
#define cbldType_Circle 0
#define cbldType_LineSeg 1
struct cbLD_Item
{
	cbIndex type; // tells how to examine cbldItem.data
	union
	{
		cbLD_Circle circle;
		cbLD_LineSeg lineseg;
	}
	data;
};
struct cbLineDrawing
{
    cbLD_Item* items;
    cbIndex itemcount;
	// in rendering,
	// points will be multiplied by cbComponentVisual.dims
	// then offset by cbComponent.pos
	// therefore, points should be in the range [-1,1]
};
struct cbComponent
{
	cbVec2 pos;
	cbLineDrawing* visual;
	cbIndex type; // tells which part of the union to examine
	union
	{
		/*cbCapacitor capacitor;
		cbResistor resistor;
		cbDiode diode;*/
	}
	data;
};
struct cbWire
{
	cbLD_LineSeg seg;
	void* at_start; // if iscmp_start, is pointer to cbComponent. otherwise, is pointer to cbWire.
	void* at_end; // if iscmp_end, is pointer to cbComponent. otherwise, is pointer to cbWire.
	char iscmp_start,iscmp_end; // BOOLEANS
};
cbVec2 cbViewportStart = {0.3,0.1};
cbVec2 cbViewpointDims = {0.7,0.9};
cbComponent** cbComponents = NULL;
cbIndex cbComponentCount = 0;
cbWire** cbWires = NULL;
cbIndex cbWireCount = 0;
#define cbListAlloc 64
cbComponent* cbCreateComponent ()
{
	if (!(cbComponentCount % cbListAlloc)) cbComponents = realloc(cbComponents,cbComponentCount + cbListAlloc); // Extend.
	cbComponent** out = cbComponents + cbComponentCount;
	cbComponentCount++;
	*out = malloc(sizeof(cbComponent));
	return *out;
};
void cbFreeComponent (cbComponent* todel)
{
	free(todel);
	cbIndex cur;
	for (cur = 0; cur < cbComponentCount; cur++)
	{
		cbComponent* item = *(cbComponents + cur);
		if (item == todel) goto RESTACK;
	};
	return;
	RESTACK:
	cbComponentCount--;
	while (cur < cbComponentCount) *(cbComponents + cur) = *(cbComponents + (cur++));
	if (!(cbComponentCount % cbListAlloc)) cbComponents = realloc(cbComponents,cbComponentCount); // Shrink.
};
cbScalar cbViewportScale = 1;
cbVec2 cbViewportOffset = {0,0};
struct cbIndexPair
{
	cbIndex x,y;
};
void cbRenderComponents ()
{
	for (cbIndex cur = 0; cur < cbComponentCount; cur++)
	{
		cbComponent* item = *(cbComponents + cur);
		for (cbIndex subcur = 0; subcur < item->visual->itemcount; subcur++)
		{
			cbLD_Item subitem = *(item->visual->items + subcur);
			cbIndexPair topleft,bottomright;
			cbIndexPair curpixel;
			switch (subitem.type)
			{
				case cbldType_Circle:
                subitem.data.circle.center.x *= cbViewportScale;
                subitem.data.circle.center.y *= cbViewportScale;
                subitem.data.circle.radius *= cbViewportScale;
                for (curpixel.y = bottomright.y; curpixel.y < topleft.y; curpixel.y++) for (curpixel.x = topleft.x; curpixel.x < bottomright.x; curpixel.x++)
                {

                };
                break;
                case cbldType_LineSeg:
                    ;
				//break;
			};
		};
	};
};
#define SDL_main main
#include <stdio.h>
#include <SDL_image.h>
#define SampleImagePath "ticktock.bmp"
int main ()
{
    if (SDL_Init(SDL_INIT_EVERYTHING)) printf("Could not initialize SDL.\n");
    SDL_Window* window = SDL_CreateWindow("Circuit Builder",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720,SDL_WINDOW_RESIZABLE);
    if (!window) printf("Could not initialize window.\n");
    SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) printf("Could not initialize renderer.\n");
    SDL_Surface* ticktock = IMG_Load(SampleImagePath);
    if (!ticktock) printf("Could not load sample image (" SampleImagePath ").\n");
    SDL_Texture* ticktocktex = SDL_CreateTextureFromSurface(renderer,ticktock);
    if (!ticktocktex) printf("Could not create texture from surface.\n");
    int image_w = ticktock->w;
    int image_h = ticktock->h;
	cbScalar imageaspect = image_w / (cbScalar)image_h;
	SDL_FreeSurface(ticktock);
	cbIndex frame_id = 0;
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
    while (true)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT) goto QUIT;
		frame_id++;
		SDL_RenderFillRect(renderer,NULL);
		int screen_w;
		int screen_h;
		SDL_GetWindowSize(window,&screen_w,&screen_h);
		cbScalar screenaspect = screen_w / (cbScalar)screen_h;
		SDL_Rect drawrect;
		if (screenaspect > imageaspect)
		{
			/// Screen is wider than image.
			drawrect.w = screen_w * (imageaspect / screenaspect);
			drawrect.x = (screen_w - drawrect.w) / 2;
			drawrect.y = 0;
			drawrect.h = screen_h;
		}
		else
		{
			/// Screen is taller than image.
			drawrect.h = screen_h * (screenaspect / imageaspect);
			drawrect.y = (screen_h - drawrect.h) / 2;
			drawrect.x = 0;
			drawrect.w = screen_w;
		};
		SDL_RenderCopy(renderer,ticktocktex,NULL,&drawrect);
		SDL_RenderPresent(renderer);
	};
	QUIT:
	SDL_DestroyTexture(ticktocktex);
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};
