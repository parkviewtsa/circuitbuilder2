#define crShowWindow
#include <circuit-render.h>
int main ()
{
	// ".cbip" for "Circuit Builder Item Prototype"
	crItem* example = crCreateItem("example-proto.cbip");
	example->posx = 0;
	example->posy = 0;
	if (example) printf("crCreateItem succeeded.\n");
	else printf("crCreateItem failed with error code [%lu].\n",crGetError());
	printf("Draw System Initialized: %s\n",crInit() ? "Nope" : "YES!");
	printf("Window: %X\n",crWindow);
	printf("Renderer: %X\n",crRenderer);
	while (!SDL_QuitRequested())
	{
		crDraw();
	};
};
