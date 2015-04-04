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

crProto** crProtos = NULL;
crIndex crProtoCount = 0;

crProto* crRequireProto (char* path);
