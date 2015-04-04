#include <circuit-render.h>
int main ()
{
	// ".cbip" for "Circuit Builder Item Prototype"
	crItem* example = crCreateItem("example-proto.cbip");
	if (example) printf("crCreateItem succeeded.\n");
	else printf("crCreateItem failed with error code [%lu].\n",crGetError());
};
