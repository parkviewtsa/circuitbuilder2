char* LastSavePath = NULL;
void OnMenuClick (char* menuname)
{
	if (!strcmp(menuname,"File|Save As"))
	{
		Save();
	}
	else if (!strcmp(menuname,"File|Save"))
	{
		if (LastSavePath) SaveToPath(LastSavePath);
		else Save();
	}
	else if (!strcmp(menuname,"Edit|Undo"))
	{
		// Undo
	};
};
