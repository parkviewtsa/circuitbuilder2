#define slDefResX 640
#define slDefResY 640
#include <slice.h>
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
            if (total - r_sq < 0) if (powf(total,0.5f) - radius >= -1) writecolor = color;
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
        Circle* circle = *(Circles + cur);
        slScalar x_sq = circle->radius->x - circle->center->x;
        x_sq *= x_sq;
        slScalar y_sq = circle->radius->y - circle->center->y;
        y_sq *= y_sq;
        slScalar radius = pow(x_sq + y_sq,0.5);
        int winx;
        SDL_GetWindowSize(slWindow,&winx,NULL);
        SDL_Rect rect;
        rect.w = radius * winx;
        SDL_Texture* tex = crDrawCircle(rect.w,{255,255,255,255});
        rect.w *= 2;
        rect.w++;
        rect.h = rect.w;
        rect.x = ((circle->center->x + (circle->center->w / 2)) - radius) * winx;
        rect.x--;
        rect.y = ((circle->center->y + (circle->center->w / 2)) - radius) * winx;
        rect.y--;
        SDL_RenderCopy(slRenderer,tex,NULL,&rect);
        SDL_DestroyTexture(tex);
    };
};
slBox* grabbed = NULL;
slBox* grabbed_aux = NULL;
slScalar grabbed_aux_DiffX,grabbed_aux_DiffY;
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
        if (slPointOnBox(circle->center,slMouseX,slMouseY))
        {
            grabbed = circle->center;
            grabbed_aux = circle->radius;
            grabbed_aux_DiffX = grabbed_aux->x - grabbed->x;
            grabbed_aux_DiffY = grabbed_aux->y - grabbed->y;
            return;
        }
        else if (slPointOnBox(circle->radius,slMouseX,slMouseY))
        {
            grabbed = circle->radius;
            return;
        };
    };
};
void Release ()
{
	grabbed = NULL;
	grabbed_aux = NULL;
};
void DeleteSomething ()
{
	Line* del_line = NULL;
	slBU cur;
	for (cur = 0; cur < LineCount; cur++)
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
		return;
	};
	Circle* del_circle = NULL;
	for (cur = 0; cur < CircleCount; cur++)
    {
        Circle* circle = *(Circles + cur);
        if (slPointOnBox(circle->center,slMouseX,slMouseY)) del_circle = circle;
        if (slPointOnBox(circle->radius,slMouseX,slMouseY)) del_circle = circle;
    };
    if (del_circle)
    {
        slRemoveItemFromList(&Circles,&CircleCount,del_circle);
        slDestroyBox(del_circle->center);
        slDestroyBox(del_circle->radius);
        free(del_circle);
    };
};
SDL_Color ClickBoxColor = {127,127,127,127};
void CreateLine ()
{
	Line* out = malloc(sizeof(Line));
	out->endpoint1 = slCreateBox(NULL);
	out->endpoint1->bordercolor = ClickBoxColor;
	out->endpoint1->w = 0.05;
	out->endpoint1->h = 0.05;
	out->endpoint1->x = 0.225;
	out->endpoint1->y = 0.475;
	out->endpoint2 = slCreateBox(NULL);
	out->endpoint2->bordercolor = ClickBoxColor;
	out->endpoint2->w = 0.05;
	out->endpoint2->h = 0.05;
	out->endpoint2->x = 0.725;
	out->endpoint2->y = 0.475;
	slAddItemToList(&Lines,&LineCount,out);
};
void CreateCircle ()
{
    Circle* out = malloc(sizeof(Circle));
    out->center = slCreateBox(NULL);
	out->center->bordercolor = ClickBoxColor;
	out->center->w = 0.05;
	out->center->h = 0.05;
	out->center->x = 0.475;
	out->center->y = 0.475;
	out->radius = slCreateBox(NULL);
	out->radius->bordercolor = ClickBoxColor;
	out->radius->w = 0.05;
	out->radius->h = 0.05;
	out->radius->x = 0.475;
	out->radius->y = 0.725;
	slAddItemToList(&Circles,&CircleCount,out);
};
void Crop ()
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
	slScalar* coords = malloc(sizeof(slScalar) * ((LineCount + CircleCount) * 4));
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
	Circle* circle;
	for (cur = 0; cur < CircleCount; cur++)
    {
		circle = *(Circles + cur);
		// Multiply by 2 and subtact 1 to go from [0,1] space to [-1,1] space.
		x1 = ((circle->center->x + (circle->center->w / 2)) * 2) - 1;
		y1 = ((circle->center->y + (circle->center->h / 2)) * 2) - 1;
		x2 = ((circle->radius->x + (circle->radius->w / 2)) * 2) - 1;
		y2 = ((circle->radius->y + (circle->radius->h / 2)) * 2) - 1;
		*coords_sub = x1;
		coords_sub++;
		*coords_sub = y1;
		coords_sub++;
		*coords_sub = x2;
		coords_sub++;
		*coords_sub = y2;
		coords_sub++;
		slScalar r = pow(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)),0.5);
		if (x1 - r < leftmost) leftmost = x1 - r;
		if (x1 - r > rightmost) rightmost = x1 - r;
		if (x1 + r < leftmost) leftmost = x1 + r;
		if (x1 + r > rightmost) rightmost = x1 + r;
		if (y1 - r < topmost) topmost = y1 - r;
		if (y1 - r > bottommost) bottommost = y1 - r;
		if (y1 + r < topmost) topmost = y1 + r;
		if (y1 + r > bottommost) bottommost = y1 + r;
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
	for (cur = 0; cur < CircleCount; cur++)
	{
		circle = *(Circles + cur);
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
		circle->center->x = ((x1 + 1) / 2) - (circle->center->w / 2);
		circle->center->y = ((y1 + 1) / 2) - (circle->center->h / 2);
		circle->radius->x = ((x2 + 1) / 2) - (circle->radius->w / 2);
		circle->radius->y = ((y2 + 1) / 2) - (circle->radius->h / 2);
	};
	// don't forget to free the buffer - avoid memory leak.
	free(coords);
	// don't free coords_sub, because it is coords
};
char* thumbnail_save_path = NULL;
void CaptureScreen ()
{
    int winx;
    SDL_GetWindowSize(slWindow,&winx,NULL);
    Uint8* pixels = malloc(winx * winx * 3);
    SDL_Rect readrect;
    readrect.x = 0;
    readrect.y = 0;
    readrect.w = winx;
    readrect.h = winx;
    SDL_RenderReadPixels(slRenderer,&readrect,SDL_PIXELFORMAT_RGB24,pixels,winx * 3);
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x00000000;
    #else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0x00000000;
    #endif
    SDL_Surface* screen = SDL_CreateRGBSurfaceFrom(pixels,winx,winx,24,winx * 3,rmask,gmask,bmask,amask);
    IMG_SavePNG(screen,thumbnail_save_path);
    free(thumbnail_save_path);
    SDL_FreeSurface(screen);
};
void Save ()
{
    FILE* file;
    char* pathtext;
    slBU pathid = 0;
    while (true)
    {
        pathtext = asprintf(&pathtext,"new-symbol-%lu.cbip",pathid);
        file = fopen(pathtext,"r");
        if (!file) break;
        free(pathtext);
    };
    asprintf(&thumbnail_save_path,"new-thumbnail-%lu.png",pathid);
	file = fopen(pathtext,"w");
	free(pathtext);
	if (file)
	{
		fprintf(file,"%u",LineCount);
		fprintf(file," %u",CircleCount);
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
            slScalar radius_x = (circle->radius->x - circle->center->x) * 2;
            radius_x *= radius_x;
            slScalar radius_y = (circle->radius->y - circle->center->y) * 2;
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
int main ()
{
	printf("Instructions...\n\n");
	printf(
		"Spawn New Line: PRESS [SPACE]\n\
		Spawn New Circle: PRESS [X]\n\
		Modify Point: HOLD [LEFT MOUSE]\n\
		Delete Line or Circle: PRESS [RIGHT MOUSE]\n\
		Save Symbol: PRESS [ENTER]\n\
		Crop Image: PRESS [C]\n\
		Snap to Axis Angles: HOLD [LEFT SHIFT]\n\
		Snap to 10x10 Grid: HOLD [LEFT CONTROL]\n\
		Snap to Other Objects: HOLD [ALT]\n\
		\n\
		If using grid snapping and object snapping,\n\
		object snapping takes precedence.\n\
		\n"
	);
	printf("Press enter to continue.\n");
	getchar();
	slInit();
	slCustomDrawStage_Middle = Draw;
	slKeyBind* click = slGetKeyBind("Modify Point",0,1);
	click->onpress = Grab;
	click->onrelease = Release;
	slGetKeyBind("New Line",SDLK_SPACE,0)->onpress = CreateLine;
	slGetKeyBind("New Circle",SDLK_x,0)->onpress = CreateCircle;
	slGetKeyBind("Delete",0,3)->onpress = DeleteSomething;
	slGetKeyBind("Save",SDLK_RETURN,0)->onpress = Save;
	slGetKeyBind("Crop",SDLK_c,0)->onpress = Crop;
	slKeyBind* snap90 = slGetKeyBind("Snap to Axis Angles",SDLK_LSHIFT,0);
	slKeyBind* snapGrid = slGetKeyBind("Snap to 10x10 Grid",SDLK_LCTRL,0);
	slKeyBind* snapObjects = slGetKeyBind("Snap to Other Objects",SDLK_LALT,0);
	while (!slExitReq)
	{
		slCycle();
		slBU cur;
		if (thumbnail_save_path)
        {
            // This can't be done inside the callback,
            // because the callback is called by slCycle,
            // so if we try to call slCycle to re-render
            // for thumbnail, then we will get some
            // very strange behaviors.
            Line* line;
            Circle* circle;
            // Make the draggable boxes go away for the moment.
            for (cur = 0; cur < LineCount; cur++)
            {
                line = *(Lines + cur);
                line->endpoint1->bordercolor = slBlankColor;
                line->endpoint2->bordercolor = slBlankColor;
            };
            for (cur = 0; cur < CircleCount; cur++)
            {
                circle = *(Circles + cur);
                circle->center->bordercolor = slBlankColor;
                circle->radius->bordercolor = slBlankColor;
            };
            // Preserve the user's preferred window size.
            int winx;
            SDL_GetWindowSize(slWindow,&winx,NULL);
            // Fixed, small thumbnail size.
            // Makes lines same thickness
            // in all thumbnails, no matter what.
            SDL_SetWindowSize(slWindow,128,128);
            slCycle(); // Render again.
            CaptureScreen(); // Save screenshot.
            // Restore the old window size.
            SDL_SetWindowSize(slWindow,winx,winx);
            // Make the draggable boxes visible again.
            for (cur = 0; cur < LineCount; cur++)
            {
                line = *(Lines + cur);
                line->endpoint1->bordercolor = ClickBoxColor;
                line->endpoint2->bordercolor = ClickBoxColor;
            };
            for (cur = 0; cur < CircleCount; cur++)
            {
                circle = *(Circles + cur);
                circle->center->bordercolor = ClickBoxColor;
                circle->radius->bordercolor = ClickBoxColor;
            };
        };
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
			if (snapGrid->down)
            {
                slScalar xten = (grabbed->x + (grabbed->w / 2)) * 10;
                slScalar yten = (grabbed->y + (grabbed->h / 2)) * 10;
                if (xten - (int)xten < 0.5) grabbed->x = ((int)xten * 0.1) - (grabbed->w / 2);
                else grabbed->x = (((int)xten + 1) * 0.1) - (grabbed->w / 2);
                if (yten - (int)yten < 0.5) grabbed->y = ((int)yten * 0.1) - (grabbed->h / 2);
                else grabbed->y = (((int)yten + 1) * 0.1) - (grabbed->h / 2);
            };
            Line* line;
            Circle* circle;
			if (grabbed_aux)
            {
                grabbed_aux->x = grabbed->x + grabbed_aux_DiffX;
                grabbed_aux->y = grabbed->y + grabbed_aux_DiffY;
            }
			else if (snap90->down)
            {
                slBox* otherbox = NULL;
                for (cur = 0; cur < LineCount; cur++)
                {
                    line = *(Lines + cur);
                    if (line->endpoint1 == grabbed)
                    {
                        otherbox = line->endpoint2;
                        break;
                    }
                    else if (line->endpoint2 == grabbed)
                    {
                        otherbox = line->endpoint1;
                        break;
                    };
                };
                if (otherbox)
                {
                    if (fabs(otherbox->x - grabbed->x) > fabs(otherbox->y - grabbed->y)) grabbed->y = otherbox->y;
                    else grabbed->x = otherbox->x;
                };
            };
            if (snapObjects->down)
            {
                for (cur = 0; cur < LineCount; cur++)
                {
                    line = *(Lines + cur);
                    if (line->endpoint1 == grabbed || line->endpoint2 == grabbed) continue;
                    if (fabs(line->endpoint1->x - grabbed->x) < grabbed->w)
                    if (fabs(line->endpoint1->y - grabbed->y) < grabbed->h)
                    {
                        grabbed->x = line->endpoint1->x;
                        grabbed->y = line->endpoint1->y;
                        goto FOUND;
                    };
                    if (fabs(line->endpoint2->x - grabbed->x) < grabbed->w)
                    if (fabs(line->endpoint2->y - grabbed->y) < grabbed->h)
                    {
                        grabbed->x = line->endpoint2->x;
                        grabbed->y = line->endpoint2->y;
                        goto FOUND;
                    };
                };
                for (cur = 0; cur < CircleCount; cur++)
                {
                    circle = *(Circles + cur);
                    if (circle->center == grabbed || circle->radius == grabbed) continue;
                    if (fabs(circle->center->x - grabbed->x) < grabbed->w)
                    if (fabs(circle->center->y - grabbed->y) < grabbed->h)
                    {
                        grabbed->x = circle->center->x;
                        grabbed->y = circle->center->y;
                        goto FOUND;
                    };
                };
                slScalar boxcornerdist = pow((grabbed->w * grabbed->w) + (grabbed->h * grabbed->h),0.5);
                for (cur = 0; cur < CircleCount; cur++)
                {
                    circle = *(Circles + cur);
                    if (circle->center == grabbed || circle->radius == grabbed) continue;
                    slScalar x_sq = circle->radius->x - circle->center->x;
                    x_sq *= x_sq;
                    slScalar y_sq = circle->radius->y - circle->center->y;
                    y_sq *= y_sq;
                    slScalar r = pow(x_sq + y_sq,0.5);
                    x_sq = grabbed->x - circle->center->x;
                    x_sq *= x_sq;
                    y_sq = grabbed->y - circle->center->y;
                    y_sq *= y_sq;
                    slScalar dist = pow(x_sq + y_sq,0.5);
                    if (fabs(dist - r) < boxcornerdist)
                    {
                        grabbed->x = circle->center->x + ((grabbed->x - circle->center->x) * (r / dist));
                        grabbed->y = circle->center->y + ((grabbed->y - circle->center->y) * (r / dist));
                        goto FOUND;
                    };
                };
                for (cur = 0; cur < LineCount; cur++)
                {
                    line = *(Lines + cur);
                    if (line->endpoint1 == grabbed || line->endpoint2 == grabbed) continue;

                };
                FOUND:
                if (grabbed_aux)
                {
                    grabbed_aux->x = grabbed->x + grabbed_aux_DiffX;
                    grabbed_aux->y = grabbed->y + grabbed_aux_DiffY;
                };
            };
		};
	};
	slQuit();
};
