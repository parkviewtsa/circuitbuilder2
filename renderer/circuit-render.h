#pragma once
#define SDL_main main

#ifdef crUseDoubles
typedef double crScalar;
#else
typedef float crScalar;
#endif
#ifdef crUseLongs
typedef unsigned long crIndex;
#else
typedef unsigned int crIndex;
#endif



struct crLine
{
	crScalar x1,y1,x2,y2;
};
struct crCircle
{
    crScalar x,y,r;
};
struct crProto
{
	crLine* lines;
	crIndex linecount;
	crCircle* circles;
	crIndex circlecount;
	char* loadedfrom;
};
/// The reason crProtos is not a single-block list is
/// so that we can let plugins load their own protos in the far future
/// without interfering with other stuff.
crProto* crRequireProto (char* path); /// If NULL, something went wrong.



struct crItem
{
	crProto* proto;
	crScalar posx,posy;
	crScalar scale;
	SDL_Color color;
};
/// The reason crItems is not a single-block list is
/// so that expanding the list of items does not break
/// the application's references to previously created items.
crItem* crCreateItem (char* path);
void crDestroyItem (crItem* todel);



#define crError_NoProtoPath 1
#define crError_NoProtoFile 2
#define crError_BadProtoFile 3
#define crError_FailedAlloc 4
#define crError_SDL_InitFailure 5
#define crError_SDL_NoWindow 6
#define crError_SDL_NoRenderer 7
#define crError_BadArgs 8
#define crError_ForcedResize 9
crIndex crGetError ();



bool crInit (); // true: failure, false: success
void crDraw ();
void crQuit ();
void crDropAll ();
void crResize (crIndex, crIndex);
void crGetImg (void**, crIndex*, crIndex*);
void* crGetImg_ForceDims (crIndex, crIndex);
void crGetViewPos (crScalar* x, crScalar* y);
void crSetViewPos (crScalar x, crScalar y);
crScalar crGetViewWidth ();
void crSetViewWidth (crScalar w);
void cbWorldToScreen (crScalar* x, crScalar* y);
void cbScreenToWorld (crScalar* x, crScalar* y);
