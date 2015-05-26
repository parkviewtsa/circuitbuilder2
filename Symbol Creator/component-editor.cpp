#define slDefResX 640
#define slDefResY 640
#include <slice.h>
#include <sliceopts.h>
struct Line
{
	slBox* endpoint1;
	slBox* endpoint2;
};
Line** Lines = NULL;
slBU LineCount = 0;
void DrawLines (int winw, int winh)
{
	SDL_SetRenderDrawColor(slRenderer,255,255,255,255);
	for (slBU cur = 0; cur < LineCount; cur++)
	{
		slBox* endpoint1 = (*(Lines + cur))->endpoint1;
		slBox* endpoint2 = (*(Lines + cur))->endpoint2;
		SDL_RenderDrawLine(
			slRenderer,
			(endpoint1->x + (endpoint1->w / 2)) * winw,(endpoint1->y + (endpoint1->h / 2)) * winh,
			(endpoint2->x + (endpoint2->w / 2)) * winw,(endpoint2->y + (endpoint2->h / 2)) * winh
		);
	};
};
slBox* grabbed = NULL;
void GrabLine ()
{
	for (slBU cur = 0; cur < LineCount; cur++)
	{
		if (slPointOnBox((*(Lines + cur))->endpoint1,slMouseX,slMouseY)) grabbed = (*(Lines + cur))->endpoint1;
		if (slPointOnBox((*(Lines + cur))->endpoint2,slMouseX,slMouseY)) grabbed = (*(Lines + cur))->endpoint2;
	};
};
void ReleaseLine ()
{
	grabbed = NULL;
};
void DeleteLine ()
{
	Line* del_line = NULL;
	for (slBU cur = 0; cur < LineCount; cur++)
	{
		Line* line = *(Lines + cur);
		if (slPointOnBox(line->endpoint1,slMouseX,slMouseY)) del_line = line;
		if (slPointOnBox(line->endpoint2,slMouseX,slMouseY)) del_line = line;
	};
	if (del_line)
	{
		slRemoveItemFromList(&Lines,&LineCount,del_line);
		slDestroyBox(del_line->endpoint1);
		slDestroyBox(del_line->endpoint2);
		free(del_line);
	};
};
void CreateLine ()
{
	Line* out = malloc(sizeof(Line));
	out->endpoint1 = slCreateBox(NULL);
	out->endpoint1->bordercolor = {255,255,255,255};
	out->endpoint1->w = 0.05;
	out->endpoint1->h = 0.05;
	out->endpoint1->x = 0.225;
	out->endpoint1->y = 0.475;
	out->endpoint2 = slCreateBox(NULL);
	out->endpoint2->bordercolor = {255,255,255,255};
	out->endpoint2->w = 0.05;
	out->endpoint2->h = 0.05;
	out->endpoint2->x = 0.725;
	out->endpoint2->y = 0.475;
	slAddItemToList(&Lines,&LineCount,out);
};
void SaveLines ()
{
	FILE* file = fopen("new-symbol.cbip","r");
	if (file)
	{
		fclose(file);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,"File I/O Warning","Saving would overwrite an existing file. Delete it first.",NULL);
		return;
	};
	file = fopen("new-symbol.cbip","w");
	if (file)
	{
		fprintf(file,"%u",LineCount);
		for (slBU cur = 0; cur < LineCount; cur++)
		{
			Line* line = *(Lines + cur);
			fprintf(
				file," %f %f %f %f",
				line->endpoint1->x + (line->endpoint1->w / 2),line->endpoint1->y + (line->endpoint1->h / 2),
				line->endpoint2->x + (line->endpoint2->w / 2),line->endpoint2->y + (line->endpoint2->h / 2)
			);
		};
		fclose(file);
	}
	else SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,"File I/O Error","Can't open output file for writing.",NULL);
};
int main ()
{
	printf("Instructions...\n\n");
	printf("Spawn New Line: PRESS SPACE\nModify Point: HOLD LEFT MOUSE\nDelete Line: PRESS RIGHT MOUSE\nSave Symbol: ENTER\n\n");
	printf("Press enter to continue.\n");
	getchar();
	slInit();
	opInit();
	slCustomDrawStage_Middle = DrawLines;
	slKeyBind* click = slGetKeyBind("Modify Endpoint",0,1);
	click->onpress = GrabLine;
	click->onrelease = ReleaseLine;
	slGetKeyBind("New Line",SDLK_SPACE,0)->onpress = CreateLine;
	slGetKeyBind("Delete Line",0,3)->onpress = DeleteLine;
	slGetKeyBind("Save",SDLK_RETURN,0)->onpress = SaveLines;
	while (!slExitReq)
	{
		slCycle();
		if (grabbed)
		{
			grabbed->x = slMouseX - (grabbed->w / 2);
			grabbed->y = slMouseY - (grabbed->h / 2);
		};
	};
	opQuit();
	slQuit();
};
