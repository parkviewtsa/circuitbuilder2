#include <circuit-render.h>
int main ()
{
	// ".cbip" for "Circuit Builder Item Prototype"
	if (crRequireProto("example-proto.cbip")) printf("crRequireProto failed with error code [%lu].\n",crGetError());
	else printf("crRequireProto succeeded.\n");
};
