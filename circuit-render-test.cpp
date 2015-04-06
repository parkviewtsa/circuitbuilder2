#define crShowWindow
#include <circuit-render.h>
/// Include the implementation specifically if not using the Code::Blocks project.
#include <circuit-render.cpp>
int main ()
{
	// ".cbip" for "Circuit Builder Item Prototype"
	crItem* example = crCreateItem("example-proto.cbip");
	example->posx = 0;
	example->posy = 0;
	if (example) printf("crCreateItem succeeded.\n");
	else printf("crCreateItem failed with error code [%lu].\n",crGetError());
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
		while (!SDL_QuitRequested()) crDraw();
		crQuit();
	};
};
