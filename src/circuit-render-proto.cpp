#include "include/circuit-render.h"

/// The reason crProtos is not a single-block list is
/// so that we can let plugins load their own protos in the far future
/// without interfering with other stuff.

crProto* crRequireProto (char* path) /// If NULL, something went wrong.
{
  if (!path)
  {
    crLastError = crError_NoProtoPath;
    return NULL;
  };
  crIndex i;
  crProto* item;
  for (i = 0; i < crProtoCount; i++)
  {
    item = *(crProtos + i);
    if (!strcmp(item->loadedfrom,path)) return item; // Already exists.
    };
    FILE* file = fopen(path,"r");
    if (!file)
    {
      crLastError = crError_NoProtoFile;
      return NULL;
    };
    crIndex linecount;
    // Get number of lines.
    if (!fscanf(file,"%lu",&linecount))
    {
      crLastError = crError_BadProtoFile;
      fclose(file); // Let it go. Else the file will be frozen until the app exits.
      return NULL;
    };
    // Get actual lines.
    crLine* lines = malloc(sizeof(crLine) * linecount);
    if (!lines)
    {
      // That memory was really important to me.
      crLastError = crError_FailedAlloc;
      fclose(file); // Let it go. Else the file will be frozen until the app exits.
      return NULL;
    };
    item = malloc(sizeof(crProto));
    if (!item)
    {
      // That memory was really important to me.
      crLastError = crError_FailedAlloc;
      fclose(file); // Let it go. Else the file will be frozen until the app exits.
      return NULL;
    };
    item->linecount = linecount;
    item->lines = lines;
    item->loadedfrom = path;
    for (i; i < linecount; i++)
    {
      crScalar x1,y1,x2,y2;
      if (fscanf(file,"%lf%lf%lf%lf",&x1,&y1,&x2,&y2) < 4)
      {
        crLastError = crError_BadProtoFile;
        fclose(file); // Let it go. Else the file will be frozen until the app exits.
        free(item); // Let's not have a memory leak on our hands.
        return NULL;
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
    *(crProtos + crProtoCount) = item;
    crProtoCount++;
    return item;
  };
