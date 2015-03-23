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
struct DropDownOption // Things under DropDownTabs, such as File->SAVE or Edit->UNDO
{
	char* text;
	Uint32 hotkey;
	SDL_Texture* icon; // icon only drawn if non-NULL. (not required)
};
struct DropDownTab // tabs at the top of screen like FILE or EDIT
{
	char* text;
	Uint32 hotkey; // used in Alt+[hotkey] as alternative to clicking
	DropDownOption* options; // only drawn if this tab is active
	cbIndex optioncount; // number of options
};
void CheckClick (int x, int y)
{
	// See if anything was clicked, and if so, take the appropriate action
};
void DrawMenu ()
{
	SDL_SetRenderDrawColor(renderer,DropDownTabColor_R,DropDownTabColor_G,DropDownTabColor_B,SDL_ALPHA_OPAQUE);
	// Draw all the tabs at the top.
	SDL_SetRenderDrawColor(renderer,DropDownItemColor_R,DropDownItemColor_G,DropDownItemColor_B,SDL_ALPHA_OPAQUE);
	// Draw all the dropped-down stuff.
};
