#pragma once
/// Everything is 100% opacity unless we want to do UI sorting to prevent artifacts. No? Didn't think so.
// down the road, the UI should be customizable in an options menu
// maybe even draggable on the fly
// but first let's finish the rest of the app
Uint8 DropDownTabColor_R = 191;
Uint8 DropDownTabColor_G = 159;
Uint8 DropDownTabColor_B = 127;
Uint8 DropDownItemColor_R = 159;
Uint8 DropDownItemColor_G = 159;
Uint8 DropDownItemColor_B = 159;
cbScalar DropDownHeight = 0.02; // height of bar and items
struct DropDownItem // Things under DropDownTabs, such as File->SAVE or Edit->UNDO
{
	char* text;
	Uint32 hotkey;
	SDL_Texture* icon; // icon only drawn if non-NULL. (not required)
	DropDownItem* subitems; // only drawn if parent tab is open
	cbIndex subcount; // number of options
	bool active;
};
DropDownItem* DropDownTabs;
cbIndex DropDownTabCount;
#define DropDownProtoPath "menus.txt"
void StringAppend (char** to, char additional)
{
	size_t len = strlen(*to);
	*to = realloc(*to,len + 2);
	*(to + len) = additional;
	*(to + (len + 1)) = '\0';
};
void AddToList (void*** items, cbIndex* itemcount, void* item)
{
	*items = realloc(*items,sizeof(void*) * (*itemcount + 1));
	*(*items + *itemcount) = item;
	(*itemcount)++;
};
void LoadDropDownMenus ()
{
	/*
		Step 1: Read File
		Step 2: Get Level
		Step 3: Get Icon
		Step 4: Get Hotkey
		Step 5: Stack All
	*/

	// Step 1 - load
	FILE* file = fopen(DropDownProtoPath,"r");
	char** protoitems = NULL;
	cbIndex protocount = 0;
	int cc;
	char* temp;
	while (true)
	{
		cc = fgetc(file);
		if (cc == EOF) break;
		if (cc == '\n')
		{
			AddToList(&protoitems,&protocount,temp);
			temp = NULL;
		}
		else StringAppend(&temp,cc);
	};
	fclose(file);

	// Step 2 - levels
	cbIndex tabcount = protocount;
	cbIndex lastlevel = 0;
	cbIndex total_toplevel = 0;
	cbIndex i,j;
	size_t len;
	char* without;
	for (i = 0; i < protocount; i++)
	{
		cbIndex level = 0;
		temp = *(protoitems + i);
		len = strlen(temp);
		for (j = 0; j < len; j++)
		{
			if (*(temp + j) != '-') break;
			level++;
		};
		if (level)
		{
			without = malloc((len + 1) - level);
			while (j < len)
			{
				*(without + (j - level)) = *(temp + j);
				j++;
			};
			*(without + (len - level)) = '\0';
			*(protoitems + i) = without;
			free(temp);
			if (level > lastlevel + 1) level = lastlevel + 1; // can not go down more than one level at a time, but probably an honest mistake
				// Append this item to last item.
		}
		else total_toplevel++;
		lastlevel = level;
	};

	// Step 3 - icons
	char** iconpaths = malloc(sizeof(char*) * protocount);
	for (i = 0; i < protocount; i++)
	{
		temp = *(protoitems + i);
		len = strlen(temp);
		for (j = 0; j < len; j++) if (*(temp + j) == '$') goto ICONPATH;
		*(iconpaths + i) = NULL;
		continue;
		ICONPATH:
		without = malloc(j + 1);
		memcpy(without,temp,j);
		*(without + j) = '\0';
		cbIndex toward = len - j;
		char* iconpath = malloc(toward);
		toward--;
		*(iconpath + toward) = '\0';
		memcpy(iconpath,temp + j + 1,toward);
		*(protoitems + i) = without;
		*(iconpaths + i) = iconpath;
		free(temp);
	};

	// Step 4 - hotkeys
	char* hotkeys = malloc(protocount);
	for (i = 0; i < protocount; i++)
	{
		temp = *(protoitems + i);
		len = strlen(temp);
		for (j = 0; j < len; j++) if (*(temp + j) == '*') goto HOTKEY;
		*(hotkeys + i) = '\0';
		HOTKEY:
		j++;
		if (j < len) *(hotkeys + i) = *(temp + j);
	};

	// Step 5 - stacking
	DropDownItem* alltabs = malloc(sizeof(DropDownItem) * protocount);
	DropDownTabCount = total_toplevel;
};
void CheckClick (int x, int y)
{
	// See if anything was clicked, and if so, take the appropriate action
};
void RenderTab (DropDownItem torender)
{
	//SDL_RenderFillRect(/*...*/);
};
void RenderSubTabs (DropDownItem* from)
{
	DropDownItem* lower = NULL;
	for (cbIndex i = 0; i < from->subcount; i++)
	{
		DropDownItem sub = *(from->subitems + i);
		if (sub.active) lower = from->subitems + i;
		RenderTab(sub);
	};
	if (lower) RenderSubTabs(lower);
};
void DrawAllTabs ()
{
	SDL_SetRenderDrawColor(renderer,DropDownTabColor_R,DropDownTabColor_G,DropDownTabColor_B,SDL_ALPHA_OPAQUE);
	cbIndex i = 0;
	for (i = 0; i < DropDownTabCount; i++)
	{
		DropDownItem* tab = DropDownTabs + i;
		if (tab->active)
		{
			SDL_SetRenderDrawColor(renderer,DropDownItemColor_R,DropDownItemColor_G,DropDownItemColor_B,SDL_ALPHA_OPAQUE);
			RenderSubTabs(tab);
		};
		RenderTab(*tab);
	};
};
void CheckMenuClick (int mousex, int mousey)
{

};
