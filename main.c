/// Later I will clean up all of this back-end stuff: organize it and make it into a single include.

#define SDL_main main

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>

#include "menu-callback.h"
#include "menu.h"

typedef unsigned long cbIndex;
typedef double cbScalar;
struct cbVec2
{
	cbScalar x,y;
};
SDL_Renderer* renderer;
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
#define SampleImagePath "ticktock.bmp"
void SaveToPath (char* path)
{
	FILE* file = fopen(path,"w");
	if (file)
	{
		fprintf(file,"#yoloswag");
		fclose(file);
	}
	else printf("Could not open file to save project.\n");
	// Save the current project to the file specified by [path].
};
void Save ()
{
	// Bring up a dialog for where to save the project. Then call SaveToPath( with the path ).
	SaveToPath("project.cbpf"); // temp path
	// ".cbpf" extension should be automatically added. "Circuit Builder Project File"
	// not ".cbp" because that would interfere with Code::Blocks project files and get annoying to us developers

	// of course, warn the user before trying to save over an existing file path!
};
SDL_Window* window;
bool AttemptQuit ()
{
	// This returns [true] to signal that the app should exit.
	const SDL_MessageBoxButtonData buttons [] =
	{
		// Ordering is funky due to the way SDL sorts by array position, not by [.buttonid].
		/* {.flags,.buttonid,.text} */
		{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,	2,	"Cancel"	},
		{0,											0,	"Discard"	},
		{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,	1,	"Save"		},
	};
	const SDL_MessageBoxColorScheme colorScheme =
	{
		/* .colors */
		{
			/* {.r,.g,.b} */
			/* SDL_MESSAGEBOX_COLOR_BACKGROUND */
			{223,223,223},
			/* SDL_MESSAGEBOX_COLOR_TEXT */
			{255,0,  0  },
			/* SDL_MESSAGEBOX_COLOR_BUTTON_BORDER */
			{0,  0,  0  },
			/* SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND */
			{191,191,191},
			/* SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED */
			{255,255,255}
		}
	};
	const SDL_MessageBoxData messageboxdata = {
		SDL_MESSAGEBOX_WARNING, /* .flags */
		window, /* .window */
		"Unsaved Changes", /* .title */
		"If you quit without saving, your unsaved work will be lost!", /* .message */
		SDL_arraysize(buttons), /* .numbuttons */
		buttons, /* .buttons */
		&colorScheme /* .colorScheme */
	};
	int buttonid;
	if (SDL_ShowMessageBox(&messageboxdata,&buttonid) < 0) printf("Error displaying message box.\n");
	else // If the message box can't be displayed, act like CANCEL was selected.
	{
		printf("Button Result: %d\n",buttonid);
		if (buttonid != 2) // #2: CANCEL
		{
			if (buttonid) Save(); // Not #0, so #1: SAVE
			return true;
		};
	};
	return false;
};
bool AltKeyDown_Left = false;
bool AltKeyDown_Right = false;
bool CheckInput ()
{
	// This returns [true] to continue the app's main loop, or [false] to kill it and cause an exit.
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT) if (AttemptQuit()) return false;
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_LALT) AltKeyDown_Left = true;
			else if (event.key.keysym.sym == SDLK_RALT) AltKeyDown_Right = true;
			else if (event.key.keysym.sym == SDLK_F4) if (AttemptQuit()) return false; // ALT+F4
			else if (AltKeyDown_Left || AltKeyDown_Right)
			{
				// Check for DropDownTab/DropDownOption hotkeys, since Alt is being held.
			};
		}
		else if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym == SDLK_LALT) AltKeyDown_Left = false;
			else if (event.key.keysym.sym == SDLK_RALT) AltKeyDown_Right = false;
		};
	};
	return true;
};

int main ()
{
    if (SDL_Init(SDL_INIT_EVERYTHING)) printf("Could not initialize SDL.\n");
    window = SDL_CreateWindow("Circuit Builder",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720,SDL_WINDOW_RESIZABLE);
    if (!window) printf("Could not initialize window.\n");
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) printf("Could not initialize renderer.\n");
    SDL_Surface* ticktock = IMG_Load(SampleImagePath);
    if (!ticktock) printf("Could not load sample image (" SampleImagePath ").\n");
    SDL_Texture* ticktocktex = SDL_CreateTextureFromSurface(renderer,ticktock);
    if (!ticktocktex) printf("Could not create texture from surface.\n");
    int image_w = ticktock->w;
    int image_h = ticktock->h;
	cbScalar imageaspect = image_w / (cbScalar)image_h;
	SDL_FreeSurface(ticktock);
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
    while (CheckInput())
	{
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
