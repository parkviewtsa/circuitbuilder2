#define slDefResX 640
#define slDefResY 640
#include <slice.h>
#include <sliceopts.h>
struct Line
{
	slBox* endpoint1;
	slBox* endpoint2;
};
SDL_Texture* crDrawCircle (slBU radius, SDL_Color color)
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
    #else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
    #endif
    slBU surfdims = (radius * 2) + 1;
    SDL_Surface* surf = SDL_CreateRGBSurface(0,surfdims,surfdims,32,rmask,gmask,bmask,amask);
    signed long r_sq = radius * radius;
    Uint8* pixel = surf->pixels;
    for (slBU y = 0; y < surfdims; y++)
    {
        slBU y_sq = y - (radius + 1);
        y_sq *= y_sq;
        for (slBU x = 0; x < surfdims; x++)
        {
            slBU x_sq = x - (radius + 1);
            x_sq *= x_sq;
            SDL_Color writecolor = {0,0,0,0};
            signed long total = y_sq + x_sq;
            if (total - r_sq < 0) if (powf(total,0.5f) - radius > -1) writecolor = color;
            *pixel = writecolor.r;
            pixel++;
            *pixel = writecolor.g;
            pixel++;
            *pixel = writecolor.b;
            pixel++;
            *pixel = writecolor.a;
            pixel++;
        };
    };
    SDL_Texture* tex = SDL_CreateTextureFromSurface(slRenderer,surf);
    SDL_FreeSurface(surf);
    return tex;
};
struct Circle
{
    slBox* center;
    slBox* radius;
};
Line** Lines = NULL;
slBU LineCount = 0;
Circle** Circles = NULL;
slBU CircleCount = 0;
void Draw (int winw, int winh)
{
	SDL_SetRenderDrawColor(slRenderer,255,255,255,255);
	slBU cur;
	for (cur = 0; cur < LineCount; cur++)
	{
		slBox* endpoint1 = (*(Lines + cur))->endpoint1;
		slBox* endpoint2 = (*(Lines + cur))->endpoint2;
		SDL_RenderDrawLine(
			slRenderer,
			(endpoint1->x + (endpoint1->w / 2)) * winw,(endpoint1->y + (endpoint1->h / 2)) * winh,
			(endpoint2->x + (endpoint2->w / 2)) * winw,(endpoint2->y + (endpoint2->h / 2)) * winh
		);
	};
	for (cur = 0; cur < CircleCount; cur++)
    {

    };
};
slBox* grabbed = NULL;
void Grab ()
{
    slBU cur;
	for (cur = 0; cur < LineCount; cur++)
	{
	    Line* line = *(Lines + cur);
        if (slPointOnBox(line->endpoint1,slMouseX,slMouseY))
        {
            grabbed = line->endpoint1;
            return;
        }
		else if (slPointOnBox(line->endpoint2,slMouseX,slMouseY))
        {
            grabbed = line->endpoint2;
            return;
        };
	};
	for (cur = 0; cur < CircleCount; cur++)
    {
        Circle* circle = *(Circles + cur);
        if (slPointOnBox(circle->center))
        {
            grabbed = line->center;
            return;
        }
        else if (slPointOnBox(circle->radius))
        {
            grabbed = line->radius;
            return;
        };
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
void CropLines ()
{
	// Start values at most likely to be overwritten.
	slScalar leftmost = 1;
	slScalar rightmost = -1;
	slScalar topmost = 1;
	slScalar bottommost = -1;
	// Vars used by both loops are defined here.
	slBU cur;
	Line* line;
	slScalar x1,y1,x2,y2;
	// Buffer this so we don't have to calculate them twice.
	slScalar* coords = malloc(sizeof(slScalar) * (LineCount * 4));
	slScalar* coords_sub = coords; // Increment this to avoid unnecessary pointer math.
	for (cur = 0; cur < LineCount; cur++)
	{
		line = *(Lines + cur);
		// Multiply by 2 and subtact 1 to go from [0,1] space to [-1,1] space.
		x1 = ((line->endpoint1->x + (line->endpoint1->w / 2)) * 2) - 1;
		y1 = ((line->endpoint1->y + (line->endpoint1->h / 2)) * 2) - 1;
		x2 = ((line->endpoint2->x + (line->endpoint2->w / 2)) * 2) - 1;
		y2 = ((line->endpoint2->y + (line->endpoint2->h / 2)) * 2) - 1;
		// Save to buffer.
		*coords_sub = x1;
		coords_sub++;
		*coords_sub = y1;
		coords_sub++;
		*coords_sub = x2;
		coords_sub++;
		*coords_sub = y2;
		coords_sub++;
		if (x1 < leftmost) leftmost = x1;
		if (x1 > rightmost) rightmost = x1;
		if (x2 < leftmost) leftmost = x2;
		if (x2 > rightmost) rightmost = x2;
		if (y1 < topmost) topmost = y1;
		if (y1 > bottommost) bottommost = y1;
		if (y2 < topmost) topmost = y2;
		if (y2 > bottommost) bottommost = y2;
	};
	slScalar width = rightmost - leftmost;
	slScalar height = bottommost - topmost;
	slScalar xmid = (rightmost + leftmost) / 2;
	slScalar ymid = (bottommost + topmost) / 2;
	// Don't warp the image - maintain aspect ratio.
	// We are using width as the scaling value.
	if (width < height) width = height; // Use height if height is larger.
	width /= 2; // Entire space is -1 to 1, which is 2.
	coords_sub = coords; // Reset to first value.
	for (cur = 0; cur < LineCount; cur++)
	{
		line = *(Lines + cur);
		// Read coords back from buffer
		x1 = *coords_sub;
		coords_sub++;
		y1 = *coords_sub;
		coords_sub++;
		x2 = *coords_sub;
		coords_sub++;
		y2 = *coords_sub;
		coords_sub++;
		// transform to crop
		x1 -= xmid;
		x2 -= xmid;
		y1 -= ymid;
		y2 -= ymid;
		x1 /= width;
		x2 /= width;
		y1 /= width;
		y2 /= width;
		// set values back into the boxes: inverse of getting values
		line->endpoint1->x = ((x1 + 1) / 2) - (line->endpoint1->w / 2);
		line->endpoint1->y = ((y1 + 1) / 2) - (line->endpoint1->h / 2);
		line->endpoint2->x = ((x2 + 1) / 2) - (line->endpoint1->w / 2);
		line->endpoint2->y = ((y2 + 1) / 2) - (line->endpoint1->h / 2);
	};
	// don't forget to free the buffer - avoid memory leak.
	free(coords);
	// don't free coords_sub, because it is coords
};
void Save ()
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
		fprintf(file,"%u",CircleCount);
		slBU cur;
		for (cur = 0; cur < LineCount; cur++)
		{
			Line* line = *(Lines + cur);
			fprintf(file," %f %f %f %f",
				((line->endpoint1->x + (line->endpoint1->w / 2)) * 2) - 1,
                ((line->endpoint1->y + (line->endpoint1->h / 2)) * 2) - 1,
				((line->endpoint2->x + (line->endpoint2->w / 2)) * 2) - 1,
                ((line->endpoint2->y + (line->endpoint2->h / 2)) * 2) - 1
			);
		};
		for (cur = 0; cur < CircleCount; cur++)
        {
            Circle* circle = *(Circles + cur);
            crScalar radius_x = (circle->radius->x - circle->center->x) * 2;
            radius_x *= radius_x;
            crScalar radius_y = (circle->radius->y - circle->center->y) * 2;
            radius_y *= radius_y;
            fprintf(file," %f %f %f",
                ((circle->center->x + (circle->center->w / 2)) * 2) - 1,
                ((circle->center->y + (circle->center->h / 2)) * 2) - 1,
                pow(radius_x + radius_y,0.5)
            );
        };
		fclose(file);
	}
	else SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,"File I/O Error","Can't open output file for writing.",NULL);
};
bool Snap90 = false;
void BeginSnap90 ()
{
    Snap90 = true;
};
void EndSnap90 ()
{
    Snap90 = false;
};
int main ()
{
	printf("Instructions...\n\n");
	printf(
		"Spawn New Line: PRESS [SPACE]\n\
		Modify Point: HOLD [LEFT MOUSE]\n\
		Delete Line: PRESS [RIGHT MOUSE]\n\
		Save Symbol: PRESS [ENTER]\n\
		Crop Image: PRESS [C]\n\
		Snap To Axis Angles: HOLD [LEFT SHIFT]\n\
		\n"
	);
	printf("Press enter to continue.\n");
	getchar();
	slInit();
	slCustomDrawStage_Middle = DrawLines;
	slKeyBind* click = slGetKeyBind("Modify Endpoint",0,1);
	click->onpress = GrabLine;
	click->onrelease = ReleaseLine;
	slGetKeyBind("New Line",SDLK_SPACE,0)->onpress = CreateLine;
	slGetKeyBind("Delete Line",0,3)->onpress = DeleteLine;
	slGetKeyBind("Save",SDLK_RETURN,0)->onpress = SaveLines;
	slGetKeyBind("Crop",SDLK_c,0)->onpress = CropLines;
	slKeyBind* snap90bind = slGetKeyBind("Snap To Axis Angles",SDLK_LSHIFT);
	snap90bind->onpress = BeginSnap90;
	snap90bind->onrelease = EndSnap90;
	while (!slExitReq)
	{
		slCycle();
		// We need the window to have a 1:1 ratio of width to height,
		// because otherwise symbols may appear warped.
		int winw,winh;
		SDL_GetWindowSize(slWindow,&winw,&winh);
		if (winw != winh)
		{
			if (winw > winh) winw = winh;
			SDL_SetWindowSize(slWindow,winw,winw);
		};
		if (grabbed)
		{
			grabbed->x = slMouseX - (grabbed->w / 2);
			grabbed->y = slMouseY - (grabbed->h / 2);
		};
	};
	slQuit();
};
