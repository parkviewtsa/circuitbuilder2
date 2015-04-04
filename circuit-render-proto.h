#pragma once
struct crLine
{
	crScalar x1,y1,x2,y2;
};
struct crProto
{
	crLine* lines;
	crIndex linecount;
	char* loadedfrom;
};
/// The reason crProtos is not a single-block list is
/// so that we can let plugins load their own protos in the far future
/// without interfering with other stuff.
crProto** crProtos = NULL;
crIndex crProtoCount = 0;
bool crRequireProto (char* path) /// Returns presence of an error. (false: no error, true: error)
{
	if (!path)
	{
		crLastError = crError_NoProtoPath;
		return true;
	};
	crIndex i;
	for (i = 0; i < crProtoCount; i++) if (!strcmp((*(crProtos + i))->loadedfrom,path)) return false; // Already exists.
	FILE* file = fopen(path,"r");
	if (!file)
	{
		crLastError = crError_NoProtoFile;
		return true;
	};
	crIndex linecount;
	// Get number of lines.
	if (!fscanf(file,"%lu",&linecount))
	{
		crLastError = crError_BadProtoFile;
		fclose(file); // Let it go. Or else something might crash and get frozen.
		return true;
	};
	// Get actual lines.
	crLine* lines = malloc(sizeof(crLine) * linecount);
	if (!lines)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		fclose(file); // Let it go. Or else something might crash and get frozen.
		return true;
	};
	crProto* out = malloc(sizeof(crProto));
	if (!out)
	{
		// That memory was really important to me.
		crLastError = crError_FailedAlloc;
		fclose(file); // Let it go. Or else something might crash and get frozen.
		return true;
	};
	out->linecount = linecount;
	out->lines = lines;
	out->loadedfrom = path;
	for (i; i < linecount; i++)
	{
		crScalar x1,y1,x2,y2;
		if (fscanf(file,"%lf%lf%lf%lf",&x1,&y1,&x2,&y2) < 4)
		{
			crLastError = crError_BadProtoFile;
			fclose(file); // Let it go. Or else something might crash and get frozen.
			return true;
		};
		lines->x1 = x1;
		lines->y1 = y1;
		lines->x2 = x2;
		lines->y2 = y2;
		lines++;
	};
	fclose(file);
	// Now add this to the archive.
	crProtos = realloc(crProtos,sizeof(void*) * (crProtoCount + 1));
	if (!crProtos)
	{
		// You done screwed it up, now. *cough* thanks a lot windows *cough*
		printf("Fatal allocation failure: crProtos main list block realloc\n");
		exit(-1);
	};
	*(crProtos + crProtoCount) = out;
	crProtoCount++;
	return false;
};
