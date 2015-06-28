#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "circuit-render.h"
/// Include the implementation specifically if not using the Code::Blocks project.
//#include "circuit-render.cpp"
int main ()
{
	// Technically we do not have to initialize the
	// circuit renderer in order to load prototypes
	// and create draw instances. The only thing we
	// can't do until it is initialized is actually
	// draw a frame.
	crItem* example = crCreateItem("example-proto.cbip");
	if (!example)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Prototype Loading Failure","Couldn't load prototype.",NULL);
        exit(0xDEADBEEF);
    };
	example->posx = -0.5;
	example->posy = -0.5;
	crSetViewWidth(2);
	// ".cbip" for "Circuit Builder Item Prototype"
	if (crInit())
	{
		// true means something went wrong
		switch (crGetError())
		{
			case crError_SDL_InitFailure:
			printf("Couldn't initialize SDL.\n");
			break;
			case crError_SDL_NoWindow:
			printf("Couldn't create a window.\n");
			break;
			case crError_SDL_NoRenderer:
			printf("Couldn't create a renderer.\n");
		};
	}
	else
	{
		// false means no error
		if (example) printf("crCreateItem succeeded.\n");
		else printf("crCreateItem failed with error code [%lu].\n",crGetError());
		while (!SDL_QuitRequested()) crDraw();
	};
	crDestroyItem(example);
		// This would actually be done by crDropAll anyway, but it looks cleaner to destroy the item specifically.
		// It is safe to call crDestroyItem with `NULL`, so even if creation of `example` failed, no problem.
	crDropAll(); // This is the only function that clears the image prototypes.
	crQuit();
		// crQuit could be instead called before destroying items.
		// crQuit is safe to call even if initialization failed.
	return 0;
};
